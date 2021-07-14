#pragma once

#include "../Common/ErrorConverter.h"
#include "../protocols/TransactionImpl.h"
#include "../protocols/TransactionReceiptImpl.h"
#include "ExecutorService.h"
#include "bcos-framework/interfaces/executor/ExecutorInterface.h"

namespace bcostars
{
class ExecutorServiceClient : public bcos::executor::ExecutorInterface
{
public:
    ExecutorServiceClient(
        bcostars::ExecutorServicePrx _proxy, bcos::crypto::CryptoSuite::Ptr _cryptoSuite)
      : m_proxy(_proxy), m_cryptoSuite(_cryptoSuite)
    {}

    ~ExecutorServiceClient() override{};
    void asyncGetCode(const std::string_view& _address,
        std::function<void(const bcos::Error::Ptr&, const std::shared_ptr<bcos::bytes>&)> _callback)
        override
    {
        class Callback : public ExecutorServicePrxCallback
        {
        public:
            Callback(
                std::function<void(const bcos::Error::Ptr&, const std::shared_ptr<bcos::bytes>&)>
                    callback)
              : m_callback(callback)
            {}

            void callback_asyncGetCode(
                const bcostars::Error& ret, const vector<tars::Char>& code) override
            {
                m_callback(toBcosError(ret), std::make_shared<bcos::bytes>(code.begin(), code.end()));
            }

            void callback_asyncGetCode_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), nullptr);
            }

        private:
            std::function<void(const bcos::Error::Ptr&, const std::shared_ptr<bcos::bytes>&)>
                m_callback;
        };

        m_proxy->async_asyncGetCode(new Callback(_callback), std::string(_address));
    }

    void asyncExecuteTransaction(const bcos::protocol::Transaction::ConstPtr& _tx,
        std::function<void(
            const bcos::Error::Ptr&, const bcos::protocol::TransactionReceipt::ConstPtr&)>
            _callback) override
    {
        class Callback : public ExecutorServicePrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&,
                         const bcos::protocol::TransactionReceipt::ConstPtr&)>
                         callback,
                bcos::crypto::CryptoSuite::Ptr cryptoSuite)
              : m_callback(callback), m_cryptoSuite(cryptoSuite)
            {}

            void callback_asyncExecuteTransaction(
                const bcostars::Error& ret, const bcostars::TransactionReceipt& receipt) override
            {
                auto bcosReceipt =
                    std::make_shared<bcostars::protocol::TransactionReceiptImpl>(m_cryptoSuite);
                bcosReceipt->setInner(receipt);

                m_callback(toBcosError(ret), bcosReceipt);
            }

            void callback_asyncExecuteTransaction_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), nullptr);
            }

        private:
            std::function<void(
                const bcos::Error::Ptr&, const bcos::protocol::TransactionReceipt::ConstPtr&)>
                m_callback;
            bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
        };

        m_proxy->async_asyncExecuteTransaction(new Callback(_callback, m_cryptoSuite),
            std::dynamic_pointer_cast<const bcostars::protocol::TransactionImpl>(_tx)->inner());
    }
    void start() override {}
    void stop() override {}

private:
    bcostars::ExecutorServicePrx m_proxy;
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
}  // namespace bcostars