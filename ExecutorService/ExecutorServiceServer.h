#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/TarsUtils.h"
#include "../DispatcherService/DispatcherServiceClient.h"
#include "../StorageService/StorageServiceClient.h"
#include "../libinitializer/ProtocolInitializer.h"
#include "../protocols/BlockImpl.h"
#include "../protocols/TransactionImpl.h"
#include "../protocols/TransactionReceiptImpl.h"
#include "Block.h"
#include "ExecutorService.h"
#include <bcos-crypto/hash/SM3.h>
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-crypto/signature/sm2/SM2Crypto.h>
#include <bcos-executor/Executor.h>
#include <bcos-framework/interfaces/executor/ExecutorInterface.h>
#include <bcos-framework/libprotocol/TransactionSubmitResultFactoryImpl.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-ledger/ledger/Ledger.h>
#include <memory>
#include <mutex>

#define EXECUTORSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[ExecutorService][Initialize]"
namespace bcostars
{
class ExecutorServiceServer : public bcostars::ExecutorService
{
public:
    void initialize() override
    {
        try
        {
            init();
            m_stopped = false;
        }
        catch (std::exception const& e)
        {
            TLOGERROR("ExecutorService init exception"
                      << LOG_KV("error", boost::diagnostic_information(e)) << std::endl);
            exit(0);
        }
        catch (tars::TC_Exception const& e)
        {
            TLOGERROR("ExecutorService init exception, error:" << e.what() << std::endl);
            exit(0);
        }
    }

    void destroy() override
    {
        if (m_stopped)
        {
            EXECUTORSERVICE_LOG(DEBUG) << LOG_DESC("the ExecutoreSerivce has already been stopped");
            return;
        }
        m_stopped = true;
        EXECUTORSERVICE_LOG(DEBUG) << LOG_DESC("stop the ExecutoreSerivce");
        if (m_executor)
        {
            m_executor->stop();
        }
        if (m_logInitializer)
        {
            m_logInitializer->stopLogging();
        }
        EXECUTORSERVICE_LOG(DEBUG) << LOG_DESC("stop the ExecutoreSerivce success");
    }

    void init()
    {
        std::call_once(m_initFlag, [this]() {
            auto configPath = ServerConfig::BasePath + "config.ini";
            TLOGINFO(LOG_DESC("ExecutorService: initLog")
                     << LOG_KV("configPath", configPath) << std::endl);
            // init the log
            boost::property_tree::ptree pt;
            boost::property_tree::read_ini(configPath, pt);
            m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
            m_logInitializer->initLog(pt);
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init log success");

            // init the nodeConfig
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init nodeConfig");
            auto keyFactory = std::make_shared<bcos::crypto::KeyFactoryImpl>();
            auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>(keyFactory);
            nodeConfig->loadConfig(configPath);
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init nodeConfig success");

            // load the gensisConfig
            auto genesisConfigPath = ServerConfig::BasePath + "config.genesis";
            EXECUTORSERVICE_LOG(INFO)
                << LOG_DESC("init GenesisConfig") << LOG_KV("configPath", genesisConfigPath);
            nodeConfig->loadGenesisConfig(genesisConfigPath);
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init GenesisConfig success");

            // load the protocol
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init protocol");
            auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
            protocolInitializer->init(nodeConfig);
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init protocol success");

            // create the storage client
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init storage service client");
            bcostars::StorageServicePrx storageServiceProxy =
                Application::getCommunicator()->stringToProxy<bcostars::StorageServicePrx>(
                    getProxyDesc(STORAGE_SERVICE_NAME));

            bcos::storage::StorageInterface::Ptr storageServiceClient =
                std::make_shared<bcostars::StorageServiceClient>(storageServiceProxy);
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init storage service client success");

            // init the ledger
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init ledger");
            auto ledger = std::make_shared<bcos::ledger::Ledger>(
                protocolInitializer->blockFactory(), storageServiceClient);
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("build the genesis block");
            // write the genesis block through ledger
            ledger->buildGenesisBlock(
                nodeConfig->ledgerConfig(), nodeConfig->txGasLimit(), nodeConfig->genesisData());
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init ledger success");

            // init the dispatcher
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init dispatcher client");
            auto dispatcherProxy =
                Application::getCommunicator()->stringToProxy<bcostars::DispatcherServicePrx>(
                    getProxyDesc(DISPATCHER_SERVANT_NAME));
            auto dispatcher = std::make_shared<bcostars::DispatcherServiceClient>(dispatcherProxy);
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("init dispatcher client success");

            // create the executor
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("create executor");
            m_executor =
                std::make_shared<bcos::executor::Executor>(protocolInitializer->blockFactory(),
                    dispatcher, ledger, storageServiceClient, nodeConfig->isWasm());
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("create executor success");
            // start the executor
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("start executor");
            m_executor->start();
            EXECUTORSERVICE_LOG(INFO) << LOG_DESC("start executor success");
        });
    }

    bcostars::Error asyncExecuteTransaction(const bcostars::Transaction& transaction,
        bcostars::TransactionReceipt& receipt, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosTransaction = std::make_shared<bcostars::protocol::TransactionImpl>(m_cryptoSuite);
        bcosTransaction->setInner(transaction);
        m_executor->asyncExecuteTransaction(
            bcosTransaction, [current](const bcos::Error::Ptr& error,
                                 const bcos::protocol::TransactionReceipt::ConstPtr& receipt) {
                if (error && error->errorCode())
                {
                    bcostars::TransactionReceipt nullobj;
                    async_response_asyncExecuteTransaction(current, toTarsError(error), nullobj);
                    return;
                }

                async_response_asyncExecuteTransaction(current, toTarsError(error),
                    std::dynamic_pointer_cast<const bcostars::protocol::TransactionReceiptImpl>(
                        receipt)
                        ->inner());
            });

        return bcostars::Error();
    }

    bcostars::Error asyncGetCode(const std::string& address, vector<tars::UInt8>& code,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_executor->asyncGetCode(address,
            [current](const bcos::Error::Ptr& error, const std::shared_ptr<bcos::bytes>& code) {
                if (error && error->errorCode())
                {
                    vector<tars::UInt8> nullobj;
                    async_response_asyncGetCode(current, toTarsError(error), nullobj);
                    return;
                }

                async_response_asyncGetCode(current, toTarsError(error), *code);
            });

        return bcostars::Error();
    }

private:
    static std::once_flag m_initFlag;
    static std::shared_ptr<bcos::executor::ExecutorInterface> m_executor;
    static bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
    static bcos::BoostLogInitializer::Ptr m_logInitializer;
    std::atomic_bool m_stopped = {false};
};
}  // namespace bcostars