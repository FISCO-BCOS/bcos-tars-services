#pragma once
#include "../Common/ErrorConverter.h"
#include "../Common/TarsUtils.h"
#include "../TxPoolService/TxPoolServiceClient.h"
#include "../libinitializer/ProtocolInitializer.h"
#include <bcos-dispatcher/DispatcherImpl.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-tars-protocol/BlockFactoryImpl.h>
#include <bcos-tars-protocol/DispatcherService.h>
#include <tarscpp/servant/Application.h>
#include <memory>


#define DISPATCHERSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[DISPATCHERSERVICE]"

namespace bcostars
{
class DispatcherServiceServer : public DispatcherService
{
public:
    void initialize() override
    {
        std::call_once(m_initFlag, [this]() {
            auto configPath = ServerConfig::BasePath + "config.ini";

            // init the log
            boost::property_tree::ptree pt;
            boost::property_tree::read_ini(configPath, pt);
            m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
            // set the boost log into the tars log directory
            m_logInitializer->setLogPath(getLogPath());
            m_logInitializer->initLog(pt);

            auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>();
            nodeConfig->loadConfig(configPath);

            auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
            protocolInitializer->init(nodeConfig);

            m_blockFactory = protocolInitializer->blockFactory();
            m_dispatcher = std::make_shared<bcos::dispatcher::DispatcherImpl>();
            // set the txpool to the dispatcher
            auto txpoolProxy =
                Application::getCommunicator()->stringToProxy<bcostars::TxPoolServicePrx>(
                    getProxyDesc(TXPOOL_SERVICE_NAME));
            auto txpool = std::make_shared<bcostars::TxPoolServiceClient>(
                txpoolProxy, protocolInitializer->cryptoSuite());
            DISPATCHERSERVICE_LOG(INFO) << LOG_DESC("init and start the dispatcher service");
            m_dispatcher->init(txpool);
            m_dispatcher->start();
            DISPATCHERSERVICE_LOG(INFO)
                << LOG_DESC("init and start the dispatcher service success");
            this->m_running = true;
        });
    }

    void destroy() override
    {
        if (!m_running)
        {
            DISPATCHERSERVICE_LOG(WARNING)
                << LOG_DESC("The dispatcher service has already been stopped");
            return;
        }
        DISPATCHERSERVICE_LOG(INFO) << LOG_DESC("Stop the dispatcher service");
        m_running = false;
        if (m_dispatcher)
        {
            m_dispatcher->stop();
        }
    }

    bcostars::Error asyncExecuteBlock(const bcostars::Block& block, tars::Bool verify,
        bcostars::BlockHeader& blockHeader, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosBlock = m_blockFactory->createBlock();
        std::dynamic_pointer_cast<bcostars::protocol::BlockImpl>(bcosBlock)->setInner(
            std::move(*const_cast<bcostars::Block*>(&block)));
        m_dispatcher->asyncExecuteBlock(bcosBlock, verify,
            [bcosBlock, current](const bcos::Error::Ptr& error,
                const bcos::protocol::BlockHeader::Ptr& blockHeader) {
                auto header = bcosBlock->blockHeader();
                if (error)
                {
                    DISPATCHERSERVICE_LOG(WARNING) << LOG_DESC("asyncExecuteBlock failed")
                                                   << LOG_KV("code", error->errorCode())
                                                   << LOG_KV("msg", error->errorMessage())
                                                   << LOG_KV("number", header->number())
                                                   << LOG_KV("hash", header->hash().abridged());
                    async_response_asyncExecuteBlock(
                        current, toTarsError(error), bcostars::BlockHeader());
                    return;
                }
                DISPATCHERSERVICE_LOG(WARNING)
                    << LOG_DESC("asyncExecuteBlock: response result")
                    << LOG_KV("number", header->number())
                    << LOG_KV("hash", header->hash().abridged())
                    << LOG_KV("hashAfterExec", blockHeader->hash().abridged());
                async_response_asyncExecuteBlock(current, toTarsError(error),
                    std::dynamic_pointer_cast<bcostars::protocol::BlockHeaderImpl>(blockHeader)
                        ->inner());
            });
        return bcostars::Error();
    }

    bcostars::Error asyncGetLatestBlock(
        bcostars::Block& block, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_dispatcher->asyncGetLatestBlock([current](const bcos::Error::Ptr& error,
                                              const bcos::protocol::Block::Ptr& block) {
            if (error)
            {
                DISPATCHERSERVICE_LOG(WARNING)
                    << LOG_DESC("asyncGetLatestBlock failed") << LOG_KV("code", error->errorCode())
                    << LOG_KV("msg", error->errorMessage());
                async_response_asyncGetLatestBlock(current, toTarsError(error), bcostars::Block());
                return;
            }
            if (block)
            {
                BCOS_LOG(INFO) << LOG_DESC("async_response_asyncGetLatestBlock")
                               << LOG_KV("number", block->blockHeader()->number());
                async_response_asyncGetLatestBlock(current, toTarsError(error),
                    std::dynamic_pointer_cast<bcostars::protocol::BlockImpl>(block)->inner());
                return;
            }
            async_response_asyncGetLatestBlock(current, toTarsError(error), bcostars::Block());
        });

        return bcostars::Error();
    }

    bcostars::Error asyncNotifyExecutionResult(const bcostars::Error& error,
        const vector<tars::Char>& orgHash, const bcostars::BlockHeader& blockHeader,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosBlockHeader = m_blockFactory->blockHeaderFactory()->createBlockHeader();
        std::dynamic_pointer_cast<bcostars::protocol::BlockHeaderImpl>(bcosBlockHeader)
            ->setInner(std::move(*const_cast<bcostars::BlockHeader*>(&blockHeader)));
        auto orgBlockHash = bcos::crypto::HashType(bcos::bytes(orgHash.begin(), orgHash.end()));
        m_dispatcher->asyncNotifyExecutionResult(toBcosError(error), orgBlockHash, bcosBlockHeader,
            [current](const bcos::Error::Ptr& error) {
                async_response_asyncNotifyExecutionResult(current, toTarsError(error));
            });
        return bcostars::Error();
    }

private:
    static std::once_flag m_initFlag;
    static bcos::dispatcher::DispatcherImpl::Ptr m_dispatcher;
    static bcostars::protocol::BlockFactoryImpl::Ptr m_blockFactory;
    static std::atomic_bool m_running;
    static bcos::BoostLogInitializer::Ptr m_logInitializer;
};

}  // namespace bcostars