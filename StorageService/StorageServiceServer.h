#pragma once

#include "../Common/TarsUtils.h"
#include "../libinitializer/ProtocolInitializer.h"
#include "ProtocolConverter.h"
#include "StorageService.h"
#include "servant/Application.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <bcos-framework/interfaces/storage/TableInterface.h>
#include <bcos-framework/libtable/TableFactory.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-ledger/libledger/Ledger.h>
#include <bcos-storage/KVDBImpl.h>
#include <bcos-storage/RocksDBAdapter.h>
#include <bcos-storage/RocksDBAdapterFactory.h>
#include <bcos-storage/Storage.h>
#include <memory>
#include <mutex>
#define STORAGESERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[StorageService][Initialize]"

namespace bcostars
{
class StorageServiceServer : public StorageService
{
public:
    ~StorageServiceServer() override {}

    void initialize() override
    {
        try
        {
            init();
            m_stopped = false;
        }
        catch (tars::TC_Exception const& e)
        {
            TLOGERROR("StorageServiceServer init exception, error:" << e.what() << std::endl);
            exit(0);
        }
        catch (std::exception const& e)
        {
            TLOGERROR("StorageServiceServer init exception"
                      << LOG_KV("error", boost::diagnostic_information(e)) << std::endl);
            exit(0);
        }
    }
    void init()
    {
        std::call_once(m_storageFlag, [this]() {
            // load the config
            auto configPath = ServerConfig::BasePath + "config.ini";
            // init the log
            boost::property_tree::ptree pt;
            boost::property_tree::read_ini(configPath, pt);
            m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
            // set the boost log into the tars log directory
            m_logInitializer->setLogPath(getLogPath());
            m_logInitializer->initLog(pt);
            STORAGESERVICE_LOG(INFO) << LOG_DESC("init log success");

            auto keyFactory = std::make_shared<bcos::crypto::KeyFactoryImpl>();
            auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>(keyFactory);
            nodeConfig->loadConfig(configPath);

            // load the protocol
            STORAGESERVICE_LOG(INFO) << LOG_DESC("init protocol");
            auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
            protocolInitializer->init(nodeConfig);
            m_cryptoSuite = protocolInitializer->cryptoSuite();
            STORAGESERVICE_LOG(INFO) << LOG_DESC("init protocol success");


            auto storagePath = ServerConfig::BasePath + nodeConfig->storagePath();
            STORAGESERVICE_LOG(INFO)
                << LOG_DESC("open DB") << LOG_KV("storageDBName", nodeConfig->storageDBName())
                << LOG_KV("stateDBName", nodeConfig->stateDBName())
                << LOG_KV("storagePath", storagePath);
            bcos::storage::RocksDBAdapterFactory rocksdbAdapterFactory(storagePath);
            auto ret = rocksdbAdapterFactory.createRocksDB(
                nodeConfig->storageDBName(), bcos::storage::RocksDBAdapter::TABLE_PERFIX_LENGTH);
            if (!ret.first)
            {
                throw std::runtime_error("createRocksDB failed!");
            }
            auto kvDB = std::make_shared<bcos::storage::KVDBImpl>(ret.first);
            auto adapter = rocksdbAdapterFactory.createAdapter(
                nodeConfig->stateDBName(), bcos::storage::RocksDBAdapter::TABLE_PERFIX_LENGTH);
            auto storageImpl = std::make_shared<bcos::storage::StorageImpl>(adapter, kvDB);
            storageImpl->disableCache();
            m_storage = storageImpl;
            STORAGESERVICE_LOG(INFO) << LOG_DESC("start the storage");
            m_storage->start();
            STORAGESERVICE_LOG(INFO) << LOG_DESC("start the storage success");

            // load the gensisConfig
            auto genesisConfigPath = ServerConfig::BasePath + "config.genesis";
            STORAGESERVICE_LOG(INFO)
                << LOG_DESC("init GenesisConfig") << LOG_KV("configPath", genesisConfigPath);
            nodeConfig->loadGenesisConfig(genesisConfigPath);
            STORAGESERVICE_LOG(INFO) << LOG_DESC("init GenesisConfig success");

            // init the ledger
            STORAGESERVICE_LOG(INFO) << LOG_DESC("init ledger");
            auto ledger = std::make_shared<bcos::ledger::Ledger>(
                protocolInitializer->blockFactory(), m_storage);
            STORAGESERVICE_LOG(INFO) << LOG_DESC("build the genesis block");
            // write the genesis block through ledger
            auto succ = ledger->buildGenesisBlock(
                nodeConfig->ledgerConfig(), nodeConfig->txGasLimit(), nodeConfig->genesisData());
            if (!succ)
            {
                STORAGESERVICE_LOG(ERROR) << LOG_DESC("build genesis failed");
                exit(0);
            }
            STORAGESERVICE_LOG(INFO) << LOG_DESC("init ledger success");
        });
    }

    void destroy() override
    {
        if (m_stopped)
        {
            STORAGESERVICE_LOG(WARNING) << LOG_DESC("the storageService has already been stopped");
            return;
        }
        STORAGESERVICE_LOG(INFO) << LOG_DESC("stop the storageService");
        m_stopped = true;
        if (m_storage)
        {
            m_storage->stop();
        }
        if (m_logInitializer)
        {
            m_logInitializer->stopLogging();
        }
    }

    bcostars::Error addStateCache(tars::Int64 blockNumber,
        const bcostars::TableFactory& tableFactory, tars::TarsCurrentPtr current) override
    {
        try
        {
            m_storage->addStateCache(
                blockNumber, toBcosTableFactory(tableFactory, m_storage, m_cryptoSuite));
        }
        catch (const bcos::Error& error)
        {
            return toTarsError(error);
        }

        return toTarsError(nullptr);
    }

    bcostars::Error commitBlock(tars::Int64 blockNumber, const vector<bcostars::TableInfo>& infos,
        const vector<map<std::string, bcostars::Entry>>& datas, tars::Int64& count,
        tars::TarsCurrentPtr current) override
    {
        auto bcosTableInfos = std::vector<bcos::storage::TableInfo::Ptr>();
        bcosTableInfos.reserve(infos.size());
        for (auto const& tableInfo : infos)
        {
            bcosTableInfos.emplace_back(std::make_shared<bcos::storage::TableInfo>(
                tableInfo.name, tableInfo._key, tableInfo.fields));
        }

        auto bcosDatas =
            std::vector<std::shared_ptr<std::map<std::string, bcos::storage::Entry::Ptr>>>();
        for (auto const& data : datas)
        {
            auto bcosData = std::make_shared<std::map<std::string, bcos::storage::Entry::Ptr>>();
            for (auto& entry : data)
            {
                auto bcosEntry = std::make_shared<bcos::storage::Entry>();
                bcosEntry->setNum(entry.second.num);
                bcosEntry->setStatus((bcos::storage::Entry::Status)entry.second.status);
                for (auto& field : entry.second.fields)
                {
                    bcosEntry->setField(std::move(field.first), std::move(field.second));
                }
                bcosData->emplace(entry.first, bcosEntry);
            }

            bcosDatas.emplace_back(bcosData);
        }

        try
        {
            auto [c, error] = m_storage->commitBlock(blockNumber, bcosTableInfos, bcosDatas);
            count = c;

            return toTarsError(error);
        }
        catch (const bcos::Error& error)
        {
            count = 0;
            return toTarsError(error);
        }
    }

    bcostars::Error dropStateCache(tars::Int64 blockNumber, tars::TarsCurrentPtr current) override
    {
        try
        {
            m_storage->dropStateCache(blockNumber);
        }
        catch (const bcos::Error& error)
        {
            return toTarsError(error);
        }

        return toTarsError(nullptr);
    }

    bcostars::Error get(const std::string& columnFamily, const std::string& _key,
        std::string& value, tars::TarsCurrentPtr current) override
    {
        try
        {
            bcos::Error::Ptr error;

            std::tie(value, error) = m_storage->get(columnFamily, _key);
            return toTarsError(error);
        }
        catch (const bcos::Error& error)
        {
            return toTarsError(error);
        }
    }

    bcostars::Error getBatch(const std::string& columnFamily, const vector<std::string>& keys,
        vector<std::string>& values, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_storage->asyncGetBatch(columnFamily, std::make_shared<vector<std::string>>(keys),
            [current, this](
                bcos::Error::Ptr error, std::shared_ptr<std::vector<std::string>> values) {
                async_response_getBatch(current, toTarsError(error), *values);
            });
        return bcostars::Error();
    }

    bcostars::Error getPrimaryKeys(const bcostars::TableInfo& tableInfo,
        const bcostars::Condition& condition, vector<std::string>& keys,
        tars::TarsCurrentPtr current) override
    {
        auto bcosTableInfo = toBcosTableInfo(tableInfo);
        auto bcosCondition = std::make_shared<bcos::storage::Condition>();  // TODO: set the values
        try
        {
            keys = m_storage->getPrimaryKeys(bcosTableInfo, bcosCondition);
            return toTarsError(nullptr);
        }
        catch (const bcos::Error& error)
        {
            return toTarsError(error);
        }
    }

    bcostars::Error getRow(const bcostars::TableInfo& tableInfo, const std::string& key,
        bcostars::Entry& row, tars::TarsCurrentPtr current) override
    {
        try
        {
            auto bcosTableInfo = toBcosTableInfo(tableInfo);
            row = toTarsEntry(m_storage->getRow(toBcosTableInfo(tableInfo), key));
        }
        catch (const bcos::Error& error)
        {
            return toTarsError(error);
        }

        return toTarsError(nullptr);
    }

    bcostars::Error getRows(const bcostars::TableInfo& tableInfo, const vector<std::string>& keys,
        map<std::string, bcostars::Entry>& rows, tars::TarsCurrentPtr current) override
    {
        try
        {
            auto bcosRows = m_storage->getRows(toBcosTableInfo(tableInfo), keys);
            for (auto const& row : bcosRows)
            {
                rows.emplace(row.first, toTarsEntry(row.second));
            }
        }
        catch (const bcos::Error& error)
        {
            return toTarsError(error);
        }

        return toTarsError(nullptr);
    }

    bcostars::Error getStateCache(tars::Int64 blockNumber, bcostars::TableFactory& tableFactory,
        tars::TarsCurrentPtr current) override
    {
        try
        {
            auto bcosTableFactory = m_storage->getStateCache(blockNumber);

            tableFactory = toTarsTableFactory(bcosTableFactory);
        }
        catch (const bcos::Error& error)
        {
            return toTarsError(error);
        }

        return toTarsError(nullptr);
    }

    bcostars::Error put(const std::string& columnFamily, const std::string& _key,
        const std::string& value, tars::TarsCurrentPtr current) override
    {
        try
        {
            return toTarsError(m_storage->put(columnFamily, _key, value));
        }
        catch (const bcos::Error& error)
        {
            return toTarsError(error);
        }
    }

    bcostars::Error remove(const std::string& columnFamily, const std::string& _key,
        tars::TarsCurrentPtr current) override
    {
        try
        {
            return toTarsError(m_storage->remove(columnFamily, _key));
        }
        catch (const bcos::Error& error)
        {
            return toTarsError(error);
        }
    }

private:
    static std::once_flag m_storageFlag;
    static bcos::storage::StorageInterface::Ptr m_storage;
    static bcos::BoostLogInitializer::Ptr m_logInitializer;
    static bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
    static std::atomic_bool m_stopped;
};

}  // namespace bcostars