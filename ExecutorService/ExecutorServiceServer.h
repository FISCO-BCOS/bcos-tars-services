#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/ProxyDesc.h"
#include "../StorageService/StorageServiceClient.h"
#include "../protocols/BlockImpl.h"
#include "../protocols/TransactionImpl.h"
#include "../protocols/TransactionReceiptImpl.h"
#include "Block.h"
#include "ExecutorService.h"
#include "bcos-crypto/hash/SM3.h"
#include "bcos-crypto/signature/key/KeyFactoryImpl.h"
#include "bcos-crypto/signature/sm2/SM2Crypto.h"
#include "bcos-executor/Executor.h"
#include "bcos-framework/interfaces/executor/ExecutorInterface.h"
#include "bcos-framework/libprotocol/TransactionSubmitResultFactoryImpl.h"
#include "bcos-ledger/ledger/Ledger.h"
#include <memory>
#include <mutex>

namespace bcostars
{
class ExecutorServiceServer : public bcostars::ExecutorService
{
public:
    void initialize() override
    {
        std::call_once(m_initFlag, [this]() {
            m_cryptoSuite =
                std::make_shared<bcos::crypto::CryptoSuite>(std::make_shared<bcos::crypto::SM3>(),
                    std::make_shared<bcos::crypto::SM2Crypto>(), nullptr);

            auto transactionFactory =
                std::make_shared<bcostars::protocol::TransactionFactoryImpl>(m_cryptoSuite);
            auto transactionReceiptFactory =
                std::make_shared<bcostars::protocol::TransactionReceiptFactoryImpl>(m_cryptoSuite);

            auto blockHeaderFactory =
                std::make_shared<bcostars::protocol::BlockHeaderFactoryImpl>(m_cryptoSuite);
            auto blockFactory = std::make_shared<bcostars::protocol::BlockFactoryImpl>(
                m_cryptoSuite, blockHeaderFactory, transactionFactory, transactionReceiptFactory);

            // TODO: add dispatcher
            // auto dispatcherProxy =
            // Application::getCommunicator()->stringToProxy<bcostars::FrontServicePrx>(getProxyDesc(DispatcherServiceObj"));

            bcostars::StorageServicePrx storageServiceProxy =
                Application::getCommunicator()->stringToProxy<bcostars::StorageServicePrx>(
                    getProxyDesc("StorageServiceObj"));
            bcos::storage::StorageInterface::Ptr storageServiceClient =
                std::make_shared<bcostars::StorageServiceClient>(storageServiceProxy);

            auto ledger =
                std::make_shared<bcos::ledger::Ledger>(blockFactory, storageServiceClient);

            bcos::dispatcher::DispatcherInterface::Ptr dispatcher;  // TODO: Init the dispatcher
            m_executor = std::make_shared<bcos::executor::Executor>(
                blockFactory, nullptr, ledger, storageServiceClient, true);
        });
    }

    void destroy() override {}

    bcostars::Error asyncExecuteTransaction(const bcostars::Transaction& transaction,
        bcostars::TransactionReceipt& receipt, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosTransaction =
            std::make_shared<bcostars::protocol::TransactionImpl>(&transaction, m_cryptoSuite);
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
};
}  // namespace bcostars