#pragma once

#include "servant/Application.h"
#include "StorageService.h"
#include "bcos-framework/interfaces/storage/Common.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include "Common.h"
#include <exception>

namespace bcostars {
class StorageServiceClient : public bcos::storage::StorageInterface {
public:
  ~StorageServiceClient() override {}

  StorageServiceClient(StorageServicePrx storageServiceProxy)
      : m_storageServiceProxy(storageServiceProxy) {}

  std::vector<std::string> getPrimaryKeys(
      const bcos::storage::TableInfo::Ptr &_tableInfo,
      const bcos::storage::Condition::Ptr &_condition) const override {
    throw bcos::Error(-1, "Unspported interface!");
  };
  bcos::storage::Entry::Ptr
  getRow(const bcos::storage::TableInfo::Ptr &_tableInfo,
         const std::string_view &_key) override {
    throw bcos::Error(-1, "Unspported interface!");
  };
  std::map<std::string, bcos::storage::Entry::Ptr>
  getRows(const bcos::storage::TableInfo::Ptr &_tableInfo,
          const std::vector<std::string> &_keys) override {
    throw bcos::Error(-1, "Unspported interface!");
  };
  std::pair<size_t, bcos::Error::Ptr> commitBlock(
      bcos::protocol::BlockNumber _blockNumber,
      const std::vector<bcos::storage::TableInfo::Ptr> &_tableInfos,
      const std::vector<
          std::shared_ptr<std::map<std::string, bcos::storage::Entry::Ptr>>>
          &_tableDatas) override {
    throw bcos::Error(-1, "Unspported interface!");
  };

  void asyncGetPrimaryKeys(const bcos::storage::TableInfo::Ptr &_tableInfo,
                           const bcos::storage::Condition::Ptr &_condition,
                           std::function<void(const bcos::Error::Ptr &,
                                              const std::vector<std::string> &)>
                               _callback) override {
    class Callback : public bcostars::StorageServiceCoroPrxCallback {
    public:
      Callback(std::function<void(const bcos::Error::Ptr &,
                                  const std::vector<std::string> &)>
                   callback,
               StorageServiceClient *self)
          : m_callback(callback), m_self(self){};

      void
      callback_getPrimaryKeys(const bcostars::Error &ret,
                              const std::vector<std::string> &keys) override {
        m_callback(m_self->toBcosError(ret), keys);
      }

      void callback_getPrimaryKeys_exception(tars::Int32 ret) override {
        m_callback(m_self->toBcosError(ret), std::vector<std::string>());
      }

    private:
      std::function<void(const bcos::Error::Ptr &,
                         const std::vector<std::string> &)>
          m_callback;
      StorageServiceClient *m_self;
    };

    m_storageServiceProxy->async_getPrimaryKeys(new Callback(_callback, this),
                                                toTarsTableInfo(_tableInfo),
                                                toTarsCondition(_condition));
  }

  virtual void
  asyncGetRow(const bcos::storage::TableInfo::Ptr &_tableInfo,
              const std::string_view &_key,
              std::function<void(const bcos::Error::Ptr &,
                                 const bcos::storage::Entry::Ptr &)>
                  _callback) override {
    class Callback : public bcostars::StorageServiceCoroPrxCallback {
    public:
      Callback(std::function<void(const bcos::Error::Ptr &,
                                  const bcos::storage::Entry::Ptr &)>
                   callback,
               StorageServiceClient *self)
          : m_callback(callback), m_self(self){};

      void callback_getRow(const bcostars::Error &ret,
                           const bcostars::Entry &row) override {
        m_callback(m_self->toBcosError(ret), m_self->toBcosEntry(row));
      }
      void callback_getRow_exception(tars::Int32 ret) override {
        m_callback(m_self->toBcosError(ret), nullptr);
      }

    private:
      std::function<void(const bcos::Error::Ptr &,
                         const bcos::storage::Entry::Ptr &)>
          m_callback;
      StorageServiceClient *m_self;
    };

    m_storageServiceProxy->async_getRow(new Callback(_callback, this),
                                        toTarsTableInfo(_tableInfo),
                                        std::string(_key));
  }
  virtual void
  asyncGetRows(const bcos::storage::TableInfo::Ptr &_tableInfo,
               const std::shared_ptr<std::vector<std::string>> &_keys,
               std::function<void(
                   const bcos::Error::Ptr &,
                   const std::map<std::string, bcos::storage::Entry::Ptr> &)>
                   _callback) override {
    class Callback : public bcostars::StorageServiceCoroPrxCallback {
    public:
      Callback(std::function<void(
                   const bcos::Error::Ptr &,
                   const std::map<std::string, bcos::storage::Entry::Ptr> &)>
                   callback,
               StorageServiceClient *self)
          : m_callback(callback), m_self(self){};

      void
      callback_getRows(const bcostars::Error &ret,
                       const map<std::string, bcostars::Entry> &rows) override {
        std::map<std::string, bcos::storage::Entry::Ptr> bcosRows;
        for (auto const &it : rows) {
          bcosRows.emplace(it.first, m_self->toBcosEntry(it.second));
        }
        m_callback(m_self->toBcosError(ret), bcosRows);
      }
      void callback_getRows_exception(tars::Int32 ret) override {
        m_callback(m_self->toBcosError(ret),
                   std::map<std::string, bcos::storage::Entry::Ptr>());
      }

    private:
      std::function<void(
          const bcos::Error::Ptr &,
          const std::map<std::string, bcos::storage::Entry::Ptr> &)>
          m_callback;
      StorageServiceClient *m_self;
    };

    m_storageServiceProxy->async_getRows(new Callback(_callback, this),
                                         toTarsTableInfo(_tableInfo), *_keys);
  }

  void asyncCommitBlock(
      bcos::protocol::BlockNumber _blockNumber,
      const std::shared_ptr<std::vector<bcos::storage::TableInfo::Ptr>> &_infos,
      const std::shared_ptr<std::vector<
          std::shared_ptr<std::map<std::string, bcos::storage::Entry::Ptr>>>>
          &_datas,
      std::function<void(const bcos::Error::Ptr &, size_t)> _callback)
      override {
    class Callback : public bcostars::StorageServiceCoroPrxCallback {
    public:
      Callback(std::function<void(const bcos::Error::Ptr &, size_t)> callback,
               StorageServiceClient *self)
          : m_callback(callback), m_self(self){};

      void callback_commitBlock(const bcostars::Error &ret,
                                tars::Int64 count) override {
        m_callback(m_self->toBcosError(ret), count);
      }
      void callback_commitBlock_exception(tars::Int32 ret) override {
        m_callback(m_self->toBcosError(ret), 0);
      }

    private:
      std::function<void(const bcos::Error::Ptr &, size_t)> m_callback;
      StorageServiceClient *m_self;
    };

    std::vector<bcostars::TableInfo> tarsTablesInfos;
    for(auto const& it: *_infos) {
      tarsTablesInfos.emplace_back(toTarsTableInfo(it));
    }

    std::vector<std::map<std::string, bcostars::Entry>> tarsDatas;
    for(auto const& it: *_datas) {
      std::map<std::string, bcostars::Entry> tableData;

      for(auto const& tableIt: *it) {
        tableData.emplace(tableIt.first, toTarsEntry(tableIt.second));
      }
    }

    m_storageServiceProxy->async_commitBlock(new Callback(_callback, this), _blockNumber, tarsTablesInfos, tarsDatas);
  }

  // cache TableFactoryInterface
  void asyncAddStateCache(
      bcos::protocol::BlockNumber _blockNumber,
      const std::shared_ptr<bcos::storage::TableFactoryInterface>
          &_tablefactory,
      std::function<void(const bcos::Error::Ptr &)> _callback) override {

      }
  virtual void asyncDropStateCache(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<void(const bcos::Error::Ptr &)> _callback) = 0;
  virtual void asyncGetStateCache(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<
          void(const bcos::Error::Ptr &,
               const std::shared_ptr<bcos::storage::TableFactoryInterface> &)>
          _callback) = 0;
  std::shared_ptr<bcos::storage::TableFactoryInterface>
  getStateCache(bcos::protocol::BlockNumber _blockNumber) override {
    throw bcos::Error(-1, "Unspported interface!");
  };
  void dropStateCache(bcos::protocol::BlockNumber _blockNumber) override {
    throw bcos::Error(-1, "Unspported interface!");
  };
  void addStateCache(bcos::protocol::BlockNumber _blockNumber,
                     const std::shared_ptr<bcos::storage::TableFactoryInterface>
                         &_tablefactory) override {
    throw bcos::Error(-1, "Unspported interface!");
  };

  // KV store in split database, used to store data off-chain
  bcos::Error::Ptr put(const std::string_view &_columnFamily,
                       const std::string_view &_key,
                       const std::string_view &_value) override {
    throw bcos::Error(-1, "Unspported interface!");
  }
  std::pair<std::string, bcos::Error::Ptr>
  get(const std::string_view &_columnFamily,
      const std::string_view &_key) override {
    throw bcos::Error(-1, "Unspported interface!");
  }
  bcos::Error::Ptr remove(const std::string_view &_columnFamily,
                          const std::string_view &_key) override {
    throw bcos::Error(-1, "Unspported interface!");
  }

  void
  asyncPut(const std::string_view &_columnFamily, const std::string_view &_key,
           const std::string_view &_value,
           std::function<void(const bcos::Error::Ptr &)> _callback) override {
    throw bcos::Error(-1, "Unspported interface!");
  }
  virtual void
  asyncRemove(const std::string_view &_columnFamily,
              const std::string_view &_key,
              std::function<void(const bcos::Error::Ptr &)> _callback) = 0;
  virtual void asyncGet(
      const std::string_view &_columnFamily, const std::string_view &_key,
      std::function<void(const bcos::Error::Ptr &, const std::string &value)>
          _callback) = 0;

  virtual void asyncGetBatch(
      const std::string_view &_columnFamily,
      const std::shared_ptr<std::vector<std::string>> &_keys,
      std::function<void(const bcos::Error::Ptr &,
                         const std::shared_ptr<std::vector<std::string>> &)>
          callback) = 0;

private:
  StorageServicePrx m_storageServiceProxy;
  const std::string m_servantName = "bcostars.StorageService.StorageServiceObj";

  bcos::Error::Ptr toBcosError(const bcostars::Error &error) const {
    if (error.errorCode == 0) {
      return nullptr;
    }

    auto bcosError =
        std::make_shared<bcos::Error>(error.errorCode, error.errorMessage);
    return bcosError;
  }

  bcos::Error::Ptr toBcosError(tars::Int32 ret) const {
    if (ret == 0) {
      return nullptr;
    }

    auto bcosError = std::make_shared<bcos::Error>(ret, "TARS error!");
    return bcosError;
  }

  bcostars::TableInfo
  toTarsTableInfo(const bcos::storage::TableInfo::Ptr &tableInfo) const {
    bcostars::TableInfo tarsTableInfo;
    tarsTableInfo.name = tableInfo->name;
    tarsTableInfo._key = tableInfo->key;
    tarsTableInfo.fields = tableInfo->fields;

    return tarsTableInfo;
  }

  bcostars::Condition
  toTarsCondition(const bcos::storage::Condition::Ptr &condition) const {
    bcostars::Condition tarsCondition;
    tarsCondition.offset = condition->getLimit().first;
    tarsCondition.size = condition->getLimit().second;
    for (auto const &it : condition->m_conditions) {
      tarsCondition.conditions.push_back((short)(it.cmp));
    }

    return tarsCondition;
  }

  bcos::storage::Entry::Ptr toBcosEntry(const bcostars::Entry &entry) const {
    auto bcosEntry = std::make_shared<bcos::storage::Entry>();
    bcosEntry->setNum(entry.num);
    bcosEntry->setStatus((bcos::storage::Entry::Status)entry.status);
    for (auto const &it : entry.fields) {
      bcosEntry->setField(it.first, it.second);
    }

    return bcosEntry;
  }
};
} // namespace bcostars