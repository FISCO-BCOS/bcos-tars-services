#pragma once

#include <bcos-framework/interfaces/storage/StorageInterface.h>

namespace bcostars {
class StorageServiceClient : public bcos::storage::StorageInterface {
public:
  ~StorageServiceClient() override {}

  std::vector<std::string> getPrimaryKeys(
      std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
      std::shared_ptr<bcos::storage::Condition> _condition) const override {};

  virtual std::shared_ptr<bcos::storage::Entry>
  getRow(std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
         const std::string_view &_key) override{};

  virtual std::map<std::string, std::shared_ptr<bcos::storage::Entry>>
  getRows(std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
          const std::vector<std::string> &_keys) override {}
  virtual size_t commitTables(
      const std::vector<std::shared_ptr<bcos::storage::TableInfo>> _tableInfos,
      std::vector<std::shared_ptr<
          std::map<std::string, std::shared_ptr<bcos::storage::Entry>>>>
          &_tableDatas) override {}

  virtual void asyncGetPrimaryKeys(
      std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
      std::shared_ptr<bcos::storage::Condition> _condition,
      std::function<void(bcos::Error, std::vector<std::string>)> _callback) override {}
  virtual void asyncGetRow(
      std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
      std::shared_ptr<std::string> _key,
      std::function<void(bcos::Error, std::shared_ptr<bcos::storage::Entry>)>
          _callback) override {}
  virtual void asyncGetRows(
      std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
      std::shared_ptr<std::vector<std::string>> _keys,
      std::function<
          void(bcos::Error bcosError,
               std::map<std::string, std::shared_ptr<bcos::storage::Entry>>)>
          _callback) override {}
  virtual void asyncCommitTables(
      std::shared_ptr<std::vector<std::shared_ptr<bcos::storage::TableInfo>>>
          _infos,
      std::shared_ptr<std::vector<
          std::shared_ptr<std::map<std::string, bcos::storage::Entry::Ptr>>>>
          &_datas,
      std::function<void(bcos::Error, size_t)> _callback) override {}

  // cache TableFactory
  virtual void
  asyncAddStateCache(bcos::protocol::BlockNumber _blockNumber,
                     bcos::protocol::Block::Ptr _block,
                     std::shared_ptr<bcos::storage::TableFactory> _tablefactory,
                     std::function<void(bcos::Error)> _callback) override {}
  virtual void
  asyncDropStateCache(bcos::protocol::BlockNumber _blockNumber,
                      std::function<void(bcos::Error)> _callback) override {}
  virtual void
  asyncGetBlock(bcos::protocol::BlockNumber _blockNumber,
                std::function<void(bcos::Error, bcos::protocol::Block::Ptr)>
                    _callback) override {}
  virtual void asyncGetStateCache(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<void(bcos::Error,
                         std::shared_ptr<bcos::storage::TableFactory>)>
          _callback) override {}
  virtual bcos::protocol::Block::Ptr
  getBlock(bcos::protocol::BlockNumber _blockNumber) override {}
  virtual std::shared_ptr<bcos::storage::TableFactory>
  getStateCache(bcos::protocol::BlockNumber _blockNumber) override {}
  virtual void dropStateCache(bcos::protocol::BlockNumber _blockNumber) override {}
  virtual void
  addStateCache(bcos::protocol::BlockNumber _blockNumber,
                bcos::protocol::Block::Ptr _block,
                std::shared_ptr<bcos::storage::TableFactory> _tablefactory) override {}
  // KV store in split database, used to store data off-chain
  virtual bool put(const std::string &columnFamily, const std::string_view &key,
                   const std::string_view &value) override {}
  virtual std::string get(const std::string &columnFamily,
                          const std::string_view &key) override {}
  virtual void
  asyncGetBatch(const std::string &columnFamily,
                std::shared_ptr<std::vector<std::string_view>> keys,
                std::function<void(bcos::Error,
                                   std::shared_ptr<std::vector<std::string>>)>
                    callback) override {}
};
} // namespace bcostars