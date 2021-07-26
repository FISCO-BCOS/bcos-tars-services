#pragma once

#include "ProtocolConverter.h"
#include "StorageService.h"
#include "bcos-framework/interfaces/storage/Common.h"
#include "servant/Application.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <emmintrin.h>
#include <exception>

#define STORAGECLIENT_LOG(LEVEL) BCOS_LOG(LEVEL) << "[StorageServiceClient]"

namespace bcostars
{
class StorageServiceClient : public bcos::storage::StorageInterface
{
public:
    ~StorageServiceClient() override {}
    StorageServiceClient(StorageServicePrx storageServiceProxy) : m_proxy(storageServiceProxy) {}

    std::vector<std::string> getPrimaryKeys(const bcos::storage::TableInfo::Ptr& _tableInfo,
        const bcos::storage::Condition::Ptr& _condition) const override
    {
        std::vector<std::string> keys;
        auto error =
            m_proxy->getPrimaryKeys(toTarsTableInfo(_tableInfo), toTarsCondition(_condition), keys);
        auto bcosError = toBcosError(error);
        if (bcosError)
        {
            STORAGECLIENT_LOG(WARNING)
                << LOG_DESC("getPrimaryKeys error") << LOG_KV("table", _tableInfo->name)
                << LOG_KV("msg", bcosError->errorMessage())
                << LOG_KV("code", bcosError->errorCode());
            return keys;
        }
        return keys;
    };

    // Note: must implement this interface for this is used by TableFactory
    bcos::storage::Entry::Ptr getRow(
        const bcos::storage::TableInfo::Ptr& _tableInfo, const std::string_view& _key) override
    {
        bcostars::Entry entry;
        auto error = m_proxy->getRow(toTarsTableInfo(_tableInfo), std::string(_key), entry);
        auto bcosError = toBcosError(error);
        if (bcosError)
        {
            STORAGECLIENT_LOG(WARNING)
                << LOG_DESC("getRow error") << LOG_KV("table", _tableInfo->name)
                << LOG_KV("key", _tableInfo->key) << LOG_KV("msg", bcosError->errorMessage())
                << LOG_KV("code", bcosError->errorCode());
            return nullptr;
        }
        return toBcosEntry(entry);
    };

    std::map<std::string, bcos::storage::Entry::Ptr> getRows(
        const bcos::storage::TableInfo::Ptr& _tableInfo,
        const std::vector<std::string>& _keys) override
    {
        std::map<std::string, bcostars::Entry> rowsRet;
        std::map<std::string, bcos::storage::Entry::Ptr> bcosRows;
        auto error = m_proxy->getRows(toTarsTableInfo(_tableInfo), _keys, rowsRet);
        auto bcosError = toBcosError(error);
        if (bcosError)
        {
            STORAGECLIENT_LOG(WARNING)
                << LOG_DESC("getRows error") << LOG_KV("table", _tableInfo->name)
                << LOG_KV("msg", bcosError->errorMessage())
                << LOG_KV("code", bcosError->errorCode());
            return bcosRows;
        }

        for (auto const& it : rowsRet)
        {
            bcosRows.emplace(it.first, toBcosEntry(it.second));
        }
        return bcosRows;
    }

    // Note: this interface is used by TableFactory to commit data
    std::pair<size_t, bcos::Error::Ptr> commitBlock(bcos::protocol::BlockNumber _blockNumber,
        const std::vector<bcos::storage::TableInfo::Ptr>& _tableInfos,
        const std::vector<std::shared_ptr<std::map<std::string, bcos::storage::Entry::Ptr>>>&
            _tableDatas) override
    {
        if (_blockNumber >= 0)
        {
            STORAGECLIENT_LOG(DEBUG) << LOG_DESC("commitBlock") << LOG_KV("number", _blockNumber);
        }

        std::vector<bcostars::TableInfo> tarsTablesInfos;
        for (auto const& it : _tableInfos)
        {
            tarsTablesInfos.emplace_back(toTarsTableInfo(it));
        }

        std::vector<std::map<std::string, bcostars::Entry>> tarsDatas;
        for (auto const& it : _tableDatas)
        {
            std::map<std::string, bcostars::Entry> tableData;
            for (auto const& tableIt : *it)
            {
                tableData.emplace(tableIt.first, toTarsEntry(tableIt.second));
            }
            tarsDatas.emplace_back(tableData);
        }
        tars::Int64 count;
        auto error = m_proxy->commitBlock(_blockNumber, tarsTablesInfos, tarsDatas, count);
        auto bcosError = toBcosError(error);
        if (bcosError)
        {
            STORAGECLIENT_LOG(WARNING)
                << LOG_DESC("commitBlock error") << LOG_KV("num", _blockNumber)
                << LOG_KV("msg", bcosError->errorMessage())
                << LOG_KV("code", bcosError->errorCode());
            return std::make_pair(0, bcosError);
        }
        if (_blockNumber >= 0)
        {
            STORAGECLIENT_LOG(INFO)
                << LOG_DESC("commitBlock success") << LOG_KV("number", _blockNumber);
        }
        return std::make_pair(count, nullptr);
    }

    void asyncGetPrimaryKeys(const bcos::storage::TableInfo::Ptr& _tableInfo,
        const bcos::storage::Condition::Ptr& _condition,
        std::function<void(const bcos::Error::Ptr&, const std::vector<std::string>&)> _callback)
        override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&, const std::vector<std::string>&)>
                    callback)
              : m_callback(callback){};

            void callback_getPrimaryKeys(
                const bcostars::Error& ret, const std::vector<std::string>& keys) override
            {
                m_callback(toBcosError(ret), keys);
            }

            void callback_getPrimaryKeys_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), std::vector<std::string>());
            }

        private:
            std::function<void(const bcos::Error::Ptr&, const std::vector<std::string>&)>
                m_callback;
        };

        m_proxy->async_getPrimaryKeys(
            new Callback(_callback), toTarsTableInfo(_tableInfo), toTarsCondition(_condition));
    }

    virtual void asyncGetRow(const bcos::storage::TableInfo::Ptr& _tableInfo,
        const std::string_view& _key,
        std::function<void(const bcos::Error::Ptr&, const bcos::storage::Entry::Ptr&)> _callback)
        override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&, const bcos::storage::Entry::Ptr&)>
                         callback,
                StorageServiceClient* self)
              : m_callback(callback), m_self(self){};

            void callback_getRow(const bcostars::Error& ret, const bcostars::Entry& row) override
            {
                m_callback(toBcosError(ret), toBcosEntry(row));
            }
            void callback_getRow_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), nullptr);
            }

        private:
            std::function<void(const bcos::Error::Ptr&, const bcos::storage::Entry::Ptr&)>
                m_callback;
            StorageServiceClient* m_self;
        };

        m_proxy->async_getRow(
            new Callback(_callback, this), toTarsTableInfo(_tableInfo), std::string(_key));
    }
    virtual void asyncGetRows(const bcos::storage::TableInfo::Ptr& _tableInfo,
        const std::shared_ptr<std::vector<std::string>>& _keys,
        std::function<void(
            const bcos::Error::Ptr&, const std::map<std::string, bcos::storage::Entry::Ptr>&)>
            _callback) override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&,
                    const std::map<std::string, bcos::storage::Entry::Ptr>&)>
                    callback)
              : m_callback(callback){};

            void callback_getRows(
                const bcostars::Error& ret, const map<std::string, bcostars::Entry>& rows) override
            {
                std::map<std::string, bcos::storage::Entry::Ptr> bcosRows;
                for (auto const& it : rows)
                {
                    bcosRows.emplace(it.first, toBcosEntry(it.second));
                }
                m_callback(toBcosError(ret), bcosRows);
            }
            void callback_getRows_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), std::map<std::string, bcos::storage::Entry::Ptr>());
            }

        private:
            std::function<void(
                const bcos::Error::Ptr&, const std::map<std::string, bcos::storage::Entry::Ptr>&)>
                m_callback;
        };

        m_proxy->async_getRows(new Callback(_callback), toTarsTableInfo(_tableInfo), *_keys);
    }

    void asyncCommitBlock(bcos::protocol::BlockNumber _blockNumber,
        const std::shared_ptr<std::vector<bcos::storage::TableInfo::Ptr>>& _infos,
        const std::shared_ptr<
            std::vector<std::shared_ptr<std::map<std::string, bcos::storage::Entry::Ptr>>>>& _datas,
        std::function<void(const bcos::Error::Ptr&, size_t)> _callback) override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&, size_t)> callback)
              : m_callback(callback){};

            void callback_commitBlock(const bcostars::Error& ret, tars::Int64 count) override
            {
                m_callback(toBcosError(ret), count);
            }
            void callback_commitBlock_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), 0);
            }

        private:
            std::function<void(const bcos::Error::Ptr&, size_t)> m_callback;
        };

        std::vector<bcostars::TableInfo> tarsTablesInfos;
        for (auto const& it : *_infos)
        {
            tarsTablesInfos.emplace_back(toTarsTableInfo(it));
        }

        std::vector<std::map<std::string, bcostars::Entry>> tarsDatas;
        for (auto const& it : *_datas)
        {
            std::map<std::string, bcostars::Entry> tableData;

            for (auto const& tableIt : *it)
            {
                tableData.emplace(tableIt.first, toTarsEntry(tableIt.second));
            }

            tarsDatas.emplace_back(tableData);
        }

        m_proxy->async_commitBlock(
            new Callback(_callback), _blockNumber, tarsTablesInfos, tarsDatas);
    }

    // cache TableFactoryInterface
    void asyncAddStateCache(bcos::protocol::BlockNumber _blockNumber,
        const std::shared_ptr<bcos::storage::TableFactoryInterface>& _tablefactory,
        std::function<void(const bcos::Error::Ptr&)> _callback) override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&)> callback)
              : m_callback(callback){};

            void callback_addStateCache(const bcostars::Error& ret) override
            {
                m_callback(toBcosError(ret));
            }
            void callback_addStateCache_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret));
            }

        private:
            std::function<void(const bcos::Error::Ptr&)> m_callback;
        };

        m_proxy->async_addStateCache(
            new Callback(_callback), _blockNumber, toTarsTableFactory(_tablefactory));
    }

    void asyncDropStateCache(bcos::protocol::BlockNumber _blockNumber,
        std::function<void(const bcos::Error::Ptr&)> _callback) override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&)> callback)
              : m_callback(callback){};

            void callback_dropStateCache(const bcostars::Error& ret) override
            {
                m_callback(toBcosError(ret));
            }

            void callback_dropStateCache_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret));
            }

        private:
            std::function<void(const bcos::Error::Ptr&)> m_callback;
        };

        m_proxy->async_dropStateCache(new Callback(_callback), _blockNumber);
    }

    void asyncGetStateCache(bcos::protocol::BlockNumber _blockNumber,
        std::function<void(
            const bcos::Error::Ptr&, const std::shared_ptr<bcos::storage::TableFactoryInterface>&)>
            _callback) override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&,
                    const std::shared_ptr<bcos::storage::TableFactoryInterface>&)>
                    callback)
              : m_callback(callback){};

            void callback_getStateCache(
                const bcostars::Error& ret, const bcostars::TableFactory& tableFactory) override
            {
                m_callback(toBcosError(ret), toBcosTableFactory(tableFactory, nullptr, nullptr));
            }
            void callback_getStateCache_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), nullptr);
            }

        private:
            std::function<void(const bcos::Error::Ptr&,
                const std::shared_ptr<bcos::storage::TableFactoryInterface>&)>
                m_callback;
        };

        m_proxy->async_getStateCache(new Callback(_callback), _blockNumber);
    }

    // Note: no need to implement this interface
    std::shared_ptr<bcos::storage::TableFactoryInterface> getStateCache(
        bcos::protocol::BlockNumber _blockNumber) override
    {
        throw bcos::Error(-1, "Unspported interface!");
    };

    // Note: no need to implement this interface
    void dropStateCache(bcos::protocol::BlockNumber _blockNumber) override
    {
        throw bcos::Error(-1, "Unspported interface!");
    };

    // Note: no need to implement this interface
    void addStateCache(bcos::protocol::BlockNumber _blockNumber,
        const std::shared_ptr<bcos::storage::TableFactoryInterface>& _tablefactory) override
    {
        throw bcos::Error(-1, "Unspported interface!");
    };

    // KV store in split database, used to store data off-chain
    // Note: this interface is useless now, no need to implement this
    bcos::Error::Ptr put(const std::string_view& _columnFamily, const std::string_view& _key,
        const std::string_view& _value) override
    {
        throw bcos::Error(-1, "Unspported interface!");
    }

    // Note: this interface is useless now, no need to implement this
    std::pair<std::string, bcos::Error::Ptr> get(
        const std::string_view& _columnFamily, const std::string_view& _key) override
    {
        throw bcos::Error(-1, "Unspported interface!");
    }

    // Note: this interface is useless now, no need to implement this
    bcos::Error::Ptr remove(
        const std::string_view& _columnFamily, const std::string_view& _key) override
    {
        throw bcos::Error(-1, "Unspported interface!");
    }

    void asyncPut(const std::string_view& _columnFamily, const std::string_view& _key,
        const std::string_view& _value,
        std::function<void(const bcos::Error::Ptr&)> _callback) override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&)> callback)
              : m_callback(callback){};

            void callback_put(const bcostars::Error& ret) override { m_callback(toBcosError(ret)); }
            void callback_put_exception(tars::Int32 ret) override { m_callback(toBcosError(ret)); }

        private:
            std::function<void(const bcos::Error::Ptr&)> m_callback;
        };

        m_proxy->async_put(new Callback(_callback), std::string(_columnFamily), std::string(_key),
            std::string(_value));
    }

    void asyncRemove(const std::string_view& _columnFamily, const std::string_view& _key,
        std::function<void(const bcos::Error::Ptr&)> _callback) override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&)> callback)
              : m_callback(callback){};

            void callback_remove(const bcostars::Error& ret) override
            {
                m_callback(toBcosError(ret));
            }
            void callback_remove_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret));
            }

        private:
            std::function<void(const bcos::Error::Ptr&)> m_callback;
        };

        m_proxy->async_remove(
            new Callback(_callback), std::string(_columnFamily), std::string(_key));
    }

    void asyncGet(const std::string_view& _columnFamily, const std::string_view& _key,
        std::function<void(const bcos::Error::Ptr&, const std::string& value)> _callback) override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(
                std::function<void(const bcos::Error::Ptr&, const std::string& value)> callback)
              : m_callback(callback){};

            void callback_get(const bcostars::Error& ret, const std::string& value) override
            {
                m_callback(toBcosError(ret), value);
            }
            void callback_get_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), std::string());
            }

        private:
            std::function<void(const bcos::Error::Ptr&, const std::string& value)> m_callback;
        };

        m_proxy->async_get(new Callback(_callback), std::string(_columnFamily), std::string(_key));
    }

    void asyncGetBatch(const std::string_view& _columnFamily,
        const std::shared_ptr<std::vector<std::string>>& _keys,
        std::function<void(
            const bcos::Error::Ptr&, const std::shared_ptr<std::vector<std::string>>&)>
            callback) override
    {
        class Callback : public bcostars::StorageServiceCoroPrxCallback
        {
        public:
            Callback(std::function<void(
                    const bcos::Error::Ptr&, const std::shared_ptr<std::vector<std::string>>&)>
                    callback)
              : m_callback(callback){};

            void callback_getBatch(
                const bcostars::Error& ret, const vector<std::string>& values) override
            {
                m_callback(toBcosError(ret), std::make_shared<std::vector<std::string>>(values));
            }
            void callback_getBatch_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), nullptr);
            }

        private:
            std::function<void(
                const bcos::Error::Ptr&, const std::shared_ptr<std::vector<std::string>>&)>
                m_callback;
        };

        m_proxy->async_getBatch(new Callback(callback), std::string(_columnFamily), *_keys);
    }

private:
    StorageServicePrx m_proxy;
};
}  // namespace bcostars