#pragma once

#include "StorageService.h"
#include "servant/Application.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include "bcos-framework/interfaces/storage/Common.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <bcos-framework/interfaces/storage/TableInterface.h>
#include <bcos-framework/libtable/TableFactory.h>
#include <bcos-storage/Storage.h>
#include <bcos-storage/RocksDBAdapter/RocksDBAdapter.h>
#include <bcos-storage/KVDBImpl.h>
#include <bcos-storage/RocksDBAdapter/RocksDBAdapterFactory.h>
#include "Common.h"
#include <memory>

namespace bcostars {

class StorageServiceServer : public StorageService {
public:
  ~StorageServiceServer() override {}

  void initialize() override {
    // load the config
    bcos::storage::RocksDBAdapterFactory rocksdbAdapterFactory(ServerConfig::DataPath + "/db");
    auto rocksdbAdapter = rocksdbAdapterFactory.createAdapter("bcos_storage");
    auto ret = rocksdbAdapterFactory.createRocksDB("kv_storage");
    auto kvDB = std::make_shared<bcos::storage::KVDBImpl>(ret.first);

    auto storageImpl = std::make_shared<bcos::storage::StorageImpl>(rocksdbAdapter, kvDB);

    m_storage = storageImpl;
  }

  void destroy() override {}

  bcostars::Error addStateCache(tars::Int64 blockNumber,
                                const bcostars::TableFactory &tableFactory,
                                tars::TarsCurrentPtr current) override {
    try {
      m_storage->addStateCache(blockNumber, toBcosTableFactory(tableFactory, m_storage, m_cryptoSuite));
    } catch (const bcos::Error &error) {
      return toTarsError(error);
    }

    return toTarsError(nullptr);
  }

  bcostars::Error
  commitBlock(tars::Int64 blockNumber, const vector<bcostars::TableInfo> &infos,
              const vector<map<std::string, bcostars::Entry>> &datas,
              tars::Int64 &count, tars::TarsCurrentPtr current) override {
    auto bcosTableInfos = std::vector<bcos::storage::TableInfo::Ptr>();
    bcosTableInfos.reserve(infos.size());
    for (auto const &tableInfo : infos) {
      bcosTableInfos.emplace_back(std::make_shared<bcos::storage::TableInfo>(
          tableInfo.name, tableInfo._key, tableInfo.fields));
    }

    auto bcosDatas = std::vector<
        std::shared_ptr<std::map<std::string, bcos::storage::Entry::Ptr>>>();
    for (auto const &data : datas) {
      auto bcosData =
          std::make_shared<std::map<std::string, bcos::storage::Entry::Ptr>>();
      for (auto &entry : data) {
        auto bcosEntry = std::make_shared<bcos::storage::Entry>();
        bcosEntry->setNum(entry.second.num);
        bcosEntry->setStatus((bcos::storage::Entry::Status)entry.second.status);
        for (auto &field : entry.second.fields) {
          bcosEntry->setField(std::move(field.first), std::move(field.second));
        }
        bcosData->emplace(entry.first, bcosEntry);
      }

      bcosDatas.emplace_back(bcosData);
    }

    try {
      auto [c, error] =
          m_storage->commitBlock(blockNumber, bcosTableInfos, bcosDatas);
      c = count;

      return toTarsError(error);
    } catch (const bcos::Error &error) {
      return toTarsError(error);
    }
  }

  bcostars::Error dropStateCache(tars::Int64 blockNumber,
                                 tars::TarsCurrentPtr current) override {
    try {
      m_storage->dropStateCache(blockNumber);
    } catch (const bcos::Error &error) {
      return toTarsError(error);
    }

    return toTarsError(nullptr);
  }

  bcostars::Error get(const std::string &columnFamily, const std::string &_key,
                      std::string &value,
                      tars::TarsCurrentPtr current) override {
    try {
      bcos::Error::Ptr error;

      std::tie(value, error) = m_storage->get(columnFamily, _key);
      return toTarsError(error);
    } catch (const bcos::Error &error) {
      return toTarsError(error);
    }
  }

  bcostars::Error getBatch(const std::string &columnFamily,
                           const vector<std::string> &keys,
                           vector<std::string> &values,
                           tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    m_storage->asyncGetBatch(
        columnFamily, std::make_shared<vector<std::string>>(keys),
        [current, this](bcos::Error::Ptr error,
                        std::shared_ptr<std::vector<std::string>> values) {
          async_response_getBatch(current, toTarsError(error), *values);
        });
  }

  bcostars::Error getPrimaryKeys(const bcostars::TableInfo &tableInfo,
                                 const bcostars::Condition &condition,
                                 vector<std::string> &keys,
                                 tars::TarsCurrentPtr current) override {
    auto bcosTableInfo = toBcosTableInfo(tableInfo);
    auto bcosCondition =
        std::make_shared<bcos::storage::Condition>(); // TODO: set the values
    try {
      keys = m_storage->getPrimaryKeys(bcosTableInfo, bcosCondition);
      return toTarsError(nullptr);
    } catch (const bcos::Error &error) {
      return toTarsError(error);
    }
  }

  bcostars::Error getRow(const bcostars::TableInfo &tableInfo,
                         const std::string &key, bcostars::Entry &row,
                         tars::TarsCurrentPtr current) override {
    try {
      auto bcosTableInfo = toBcosTableInfo(tableInfo);
      row = toTarsEntry(m_storage->getRow(toBcosTableInfo(tableInfo), key));
    } catch (const bcos::Error &error) {
      return toTarsError(error);
    }

    return toTarsError(nullptr);
  }

  bcostars::Error getRows(const bcostars::TableInfo &tableInfo,
                          const vector<std::string> &keys,
                          map<std::string, bcostars::Entry> &rows,
                          tars::TarsCurrentPtr current) override {
    try {
      auto bcosRows = m_storage->getRows(toBcosTableInfo(tableInfo), keys);
      for (auto const &row : bcosRows) {
        rows.emplace(row.first, toTarsEntry(row.second));
      }
    } catch (const bcos::Error &error) {
      return toTarsError(error);
    }

    return toTarsError(nullptr);
  }

  bcostars::Error getStateCache(tars::Int64 blockNumber,
                                bcostars::TableFactory &tableFactory,
                                tars::TarsCurrentPtr current) override {
    try {
      m_storage->getStateCache(blockNumber);
    } catch (const bcos::Error &error) {
      return toTarsError(error);
    }

    return toTarsError(nullptr);
  }

  bcostars::Error put(const std::string &columnFamily, const std::string &_key,
                      const std::string &value,
                      tars::TarsCurrentPtr current) override {}

  bcostars::Error remove(const std::string &columnFamily,
                         const std::string &_key,
                         tars::TarsCurrentPtr current) override {}

private:
  bcos::storage::StorageInterface::Ptr m_storage;
  bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};

} // namespace bcostars