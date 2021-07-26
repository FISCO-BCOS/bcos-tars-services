#pragma once

#include "../Common/ErrorConverter.h"
#include "../protocols/BlockHeaderImpl.h"
#include "../protocols/BlockImpl.h"
#include "DispatcherService.h"
#include "bcos-framework/interfaces/dispatcher/DispatcherInterface.h"
#include <memory>

namespace bcostars
{
class DispatcherServiceClient : public bcos::dispatcher::DispatcherInterface
{
public:
    DispatcherServiceClient(
        bcostars::DispatcherServicePrx _proxy, bcos::protocol::BlockFactory::Ptr _blockFactory)
      : m_proxy(_proxy), m_blockFactory(_blockFactory)
    {}
    ~DispatcherServiceClient() override {}

    void asyncExecuteBlock(const bcos::protocol::Block::Ptr& _block, bool _verify,
        std::function<void(const bcos::Error::Ptr&, const bcos::protocol::BlockHeader::Ptr&)>
            _callback) override
    {
        class Callback : public bcostars::DispatcherServicePrxCallback
        {
        public:
            Callback(std::function<void(
                         const bcos::Error::Ptr&, const bcos::protocol::BlockHeader::Ptr&)>
                         callback,
                bcos::protocol::BlockHeaderFactory::Ptr blockHeaderFactory)
              : m_callback(callback), m_blockHeaderFactory(blockHeaderFactory)
            {}

            void callback_asyncExecuteBlock(
                const bcostars::Error& ret, const bcostars::BlockHeader& blockHeader) override
            {
                auto bcosBlockHeader = m_blockHeaderFactory->createBlockHeader();
                std::dynamic_pointer_cast<bcostars::protocol::BlockHeaderImpl>(bcosBlockHeader)
                    ->setInner(blockHeader);

                m_callback(toBcosError(ret), bcosBlockHeader);
            }
            void callback_asyncExecuteBlock_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), nullptr);
            }

        private:
            std::function<void(const bcos::Error::Ptr&, const bcos::protocol::BlockHeader::Ptr&)>
                m_callback;
            bcos::protocol::BlockHeaderFactory::Ptr m_blockHeaderFactory;
        };

        m_proxy->async_asyncExecuteBlock(
            new Callback(_callback, m_blockFactory->blockHeaderFactory()),
            std::dynamic_pointer_cast<bcostars::protocol::BlockImpl>(_block)->inner(), _verify);
    }

    void asyncGetLatestBlock(
        std::function<void(const bcos::Error::Ptr&, const bcos::protocol::Block::Ptr&)> _callback)
        override
    {
        class Callback : public bcostars::DispatcherServicePrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&, const bcos::protocol::Block::Ptr&)>
                         callback,
                bcos::protocol::BlockFactory::Ptr blockFactory)
              : m_callback(callback), m_blockFactory(blockFactory)
            {}

            void callback_asyncGetLatestBlock(
                const bcostars::Error& ret, const bcostars::Block& block) override
            {
                auto bcosBlock = m_blockFactory->createBlock();
                std::dynamic_pointer_cast<bcostars::protocol::BlockImpl>(bcosBlock)->setInner(
                    block);
                BCOS_LOG(INFO) << LOG_DESC("callback_asyncGetLatestBlock")
                               << LOG_KV("number", bcosBlock->blockHeader()->number())
                               << LOG_KV("hash", bcosBlock->blockHeader()->hash().abridged());
                m_callback(toBcosError(ret), bcosBlock);
            }

            void callback_asyncGetLatestBlock_exception(tars::Int32 ret) override
            {
                BCOS_LOG(INFO) << LOG_DESC("callback_asyncGetLatestBlock exception");
                m_callback(toBcosError(ret), nullptr);
            }

        private:
            std::function<void(const bcos::Error::Ptr&, const bcos::protocol::Block::Ptr&)>
                m_callback;
            bcos::protocol::BlockFactory::Ptr m_blockFactory;
        };
        m_proxy->tars_set_timeout(1000)->async_asyncGetLatestBlock(
            new Callback(_callback, m_blockFactory));
    }

    void asyncNotifyExecutionResult(const bcos::Error::Ptr& _error,
        bcos::crypto::HashType const& _orgHash, const bcos::protocol::BlockHeader::Ptr& _header,
        std::function<void(const bcos::Error::Ptr&)> _callback) override
    {
        class Callback : public bcostars::DispatcherServicePrxCallback
        {
        public:
            Callback(std::function<void(const bcos::Error::Ptr&)> callback) : m_callback(callback)
            {}

            void callback_asyncNotifyExecutionResult(const bcostars::Error& ret) override
            {
                m_callback(toBcosError(ret));
            }

            void callback_asyncNotifyExecutionResult_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret));
            }

        private:
            std::function<void(const bcos::Error::Ptr&)> m_callback;
        };

        m_proxy->async_asyncNotifyExecutionResult(new Callback(_callback), toTarsError(_error),
            std::vector<char>(_orgHash.begin(), _orgHash.end()),
            std::dynamic_pointer_cast<bcostars::protocol::BlockHeaderImpl>(_header)->inner());
    }

    void start() override {}
    void stop() override {}

private:
    bcostars::DispatcherServicePrx m_proxy;
    bcos::protocol::BlockFactory::Ptr m_blockFactory;
};
}  // namespace bcostars