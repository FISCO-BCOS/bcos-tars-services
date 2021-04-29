#pragma once

#include "StorageService.h"
#include "servant/Application.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <exception>
#include <type_traits>

namespace bcostars {
class StorageServiceClient : public bcos::storage::StorageInterface {
public:
  StorageServiceClient(CommunicatorPtr &communicator)
      : m_communicator(communicator) {}
  ~StorageServiceClient() override {}

  std::vector<std::string> getPrimaryKeys(
      std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
      std::shared_ptr<bcos::storage::Condition> _condition) const override {
    bcostars::TableInfo tableInfo;
    tableInfo.name = _tableInfo->name;
    tableInfo.keyValue = _tableInfo->key;
    tableInfo.fields = _tableInfo->fields;
    tableInfo.indices = _tableInfo->indices;
    tableInfo.enableCache = _tableInfo->enableCache;
    tableInfo.enableConsensus = _tableInfo->enableConsensus;

    bcostars::Condition condition;
    (void)_condition;

    try {
      return getProxy().getPrimaryKeys(tableInfo, condition);
    } catch (std::exception &e) {
      throw e;
    }
  };

  std::shared_ptr<bcos::storage::Entry>
  getRow(std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
         const std::string_view &_key) override{};

  std::map<std::string, std::shared_ptr<bcos::storage::Entry>>
  getRows(std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
          const std::vector<std::string> &_keys) override {}
  size_t commitTables(
      const std::vector<std::shared_ptr<bcos::storage::TableInfo>> _tableInfos,
      std::vector<std::shared_ptr<
          std::map<std::string, std::shared_ptr<bcos::storage::Entry>>>>
          &_tableDatas) override {}

  void
  asyncGetPrimaryKeys(std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
                      std::shared_ptr<bcos::storage::Condition> _condition,
                      std::function<void(bcos::Error, std::vector<std::string>)>
                          _callback) override {
    class Callback : public bcostars::StorageServicePrxCallback {
    public:
      Callback(
          std::function<void(bcos::Error, std::vector<std::string>)> callback)
          : m_callback(callback){};

      void callback_getPrimaryKeys(const vector<std::string> &keys) override {
        m_callback(bcos::Error(), keys);
      }

      void callback_getPrimaryKeys_exception(tars::Int32 ret) override {
        m_callback(bcos::Error(-1, "Error while: " +
                                       boost::lexical_cast<std::string>(ret)),
                   std::vector<std::string>());
      }

    private:
      std::function<void(bcos::Error, std::vector<std::string>)> m_callback;
    };

    bcostars::TableInfo tableInfo;
    tableInfo.name = _tableInfo->name;
    tableInfo.keyValue = _tableInfo->key;
    tableInfo.fields = _tableInfo->fields;
    tableInfo.indices = _tableInfo->indices;
    tableInfo.enableCache = _tableInfo->enableCache;
    tableInfo.enableConsensus = _tableInfo->enableConsensus;

    bcostars::Condition condition;
    (void)_condition;

    getProxy().async_getPrimaryKeys(new Callback(_callback), tableInfo,
                                  condition);
  }
  void asyncGetRow(
      std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
      std::shared_ptr<std::string> _key,
      std::function<void(bcos::Error, std::shared_ptr<bcos::storage::Entry>)>
          _callback) override {}
  void asyncGetRows(
      std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
      std::shared_ptr<std::vector<std::string>> _keys,
      std::function<
          void(bcos::Error bcosError,
               std::map<std::string, std::shared_ptr<bcos::storage::Entry>>)>
          _callback) override {}
  void asyncCommitTables(
      std::shared_ptr<std::vector<std::shared_ptr<bcos::storage::TableInfo>>>
          _infos,
      std::shared_ptr<std::vector<
          std::shared_ptr<std::map<std::string, bcos::storage::Entry::Ptr>>>>
          &_datas,
      std::function<void(bcos::Error, size_t)> _callback) override {}

  // cache TableFactory
  void
  asyncAddStateCache(bcos::protocol::BlockNumber _blockNumber,
                     bcos::protocol::Block::Ptr _block,
                     std::shared_ptr<bcos::storage::TableFactory> _tablefactory,
                     std::function<void(bcos::Error)> _callback) override {}
  void
  asyncDropStateCache(bcos::protocol::BlockNumber _blockNumber,
                      std::function<void(bcos::Error)> _callback) override {}
  void
  asyncGetBlock(bcos::protocol::BlockNumber _blockNumber,
                std::function<void(bcos::Error, bcos::protocol::Block::Ptr)>
                    _callback) override {}
  void asyncGetStateCache(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<void(bcos::Error,
                         std::shared_ptr<bcos::storage::TableFactory>)>
          _callback) override {}
  bcos::protocol::Block::Ptr
  getBlock(bcos::protocol::BlockNumber _blockNumber) override {}
  std::shared_ptr<bcos::storage::TableFactory>
  getStateCache(bcos::protocol::BlockNumber _blockNumber) override {}
  void dropStateCache(bcos::protocol::BlockNumber _blockNumber) override {}
  void addStateCache(
      bcos::protocol::BlockNumber _blockNumber,
      bcos::protocol::Block::Ptr _block,
      std::shared_ptr<bcos::storage::TableFactory> _tablefactory) override {}
  // KV store in split database, used to store data off-chain
  bool put(const std::string &columnFamily, const std::string_view &key,
           const std::string_view &value) override {}
  std::string get(const std::string &columnFamily,
                  const std::string_view &key) override {}
  void
  asyncGetBatch(const std::string &columnFamily,
                std::shared_ptr<std::vector<std::string_view>> keys,
                std::function<void(bcos::Error,
                                   std::shared_ptr<std::vector<std::string>>)>
                    callback) override {}

private:
  bcostars::StorageServiceProxy getProxy() const {
    return m_communicator->stringToProxy<StorageServiceProxy>(
        "bcostars.StorageService.StorageServiceObj");
  }

  CommunicatorPtr m_communicator;
};
} // namespace bcostars