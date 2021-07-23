#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/TarsUtils.h"
#include "../ExecutorService/ExecutorServiceClient.h"
#include "../GatewayService/GatewayServiceClient.h"
#include "../PBFTService/PBFTServiceClient.h"
#include "../StorageService/StorageServiceClient.h"
#include "../TxPoolService/TxPoolServiceClient.h"
#include "../libinitializer/ProtocolInitializer.h"
#include "Common.h"
#include "ExecutorService.h"
#include "PBFTService.h"
#include "RpcService.h"
#include "StorageService.h"
#include "servant/Servant.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-framework/interfaces/rpc/RPCInterface.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-ledger/libledger/Ledger.h>
#include <bcos-rpc/rpc/RpcFactory.h>
#include <memory>
#include <utility>

#define RPCSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[RPCSERVICE][INITIALIZER]"
#define RPCSERVICE_BADGE "[RPCSERVICE]"
namespace bcostars
{
class RpcServiceServer : public bcostars::RpcService
{
public:
    void initialize() override
    {
        try
        {
            std::call_once(m_initFlag, [this]() {
                init();
                m_running = true;
            });
        }
        catch (std::exception const& e)
        {
            TLOGERROR("init the RpcService exceptioned"
                      << LOG_KV("error", boost::diagnostic_information(e)) << std::endl);
            exit(0);
        }
    }

    void init()
    {
        // load the configuration
        auto configPath = ServerConfig::BasePath + "config.ini";
        boost::property_tree::ptree pt;
        boost::property_tree::read_ini(configPath, pt);

        TLOGINFO(
            RPCSERVICE_BADGE << LOG_DESC("init") << LOG_KV("configPath", configPath) << std::endl);

        // init log
        RPCSERVICE_LOG(INFO) << LOG_DESC("init log");
        m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
        m_logInitializer->setLogPath(getLogPath());
        m_logInitializer->initLog(pt);
        RPCSERVICE_LOG(INFO) << LOG_DESC("init log success");

        // init node config
        RPCSERVICE_LOG(INFO) << LOG_DESC("init node config");
        auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>(
            std::make_shared<bcos::crypto::KeyFactoryImpl>());
        nodeConfig->loadConfig(configPath);
        RPCSERVICE_LOG(INFO) << LOG_DESC("init node config success");

        // init rpc config
        RPCSERVICE_LOG(INFO) << LOG_DESC("init rpc config");
        auto config = std::make_shared<bcos::rpc::RpcConfig>();
        config->initConfig(configPath);
        RPCSERVICE_LOG(INFO) << LOG_DESC("init rpc config success");

        // TODO: init node info
        auto nodeInfo = std::make_shared<bcos::rpc::NodeInfo>();
        nodeInfo->chainID = nodeConfig->chainId();
        nodeInfo->groupID = nodeConfig->groupId();

        RPCSERVICE_LOG(INFO) << LOG_DESC("init rpc factory");
        auto factory = initRpcFactory(nodeConfig);
        RPCSERVICE_LOG(INFO) << LOG_DESC("init rpc factory success");

        RPCSERVICE_LOG(INFO) << LOG_DESC("start rpc");
        m_rpcInterface = factory->buildRpc(*config, *nodeInfo);
        m_rpcInterface->start();
        RPCSERVICE_LOG(INFO) << LOG_DESC("start rpc success");

        TLOGINFO(RPCSERVICE_BADGE << LOG_DESC("the rpc service success") << std::endl);
    }

    bcos::ledger::Ledger::Ptr initLedger(
        bcos::initializer::ProtocolInitializer::Ptr protocolInitializer)
    {
        // init the storage client
        RPCSERVICE_LOG(INFO) << LOG_DESC("init storage service client");
        bcostars::StorageServicePrx storageServiceProxy =
            Application::getCommunicator()->stringToProxy<bcostars::StorageServicePrx>(
                getProxyDesc(STORAGE_SERVICE_NAME));

        bcos::storage::StorageInterface::Ptr storageServiceClient =
            std::make_shared<bcostars::StorageServiceClient>(storageServiceProxy);
        RPCSERVICE_LOG(INFO) << LOG_DESC("init storage service client success");

        // init the ledger
        RPCSERVICE_LOG(INFO) << LOG_DESC("init ledger");
        auto ledger = std::make_shared<bcos::ledger::Ledger>(
            protocolInitializer->blockFactory(), storageServiceClient);
        RPCSERVICE_LOG(INFO) << LOG_DESC("init ledger success");

        return ledger;
    }

    bcos::rpc::RpcFactory::Ptr initRpcFactory(bcos::tool::NodeConfig::Ptr nodeConfig)
    {
        RPCSERVICE_LOG(INFO) << LOG_DESC("create rpc factory");

        // init the protocol
        RPCSERVICE_LOG(INFO) << LOG_DESC("init protocol");
        auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
        protocolInitializer->init(nodeConfig);
        RPCSERVICE_LOG(INFO) << LOG_DESC("init protocol success");

        // gateway
        auto gateWayProxy = Application::getCommunicator()->stringToProxy<GatewayServicePrx>(
            getProxyDesc(GATEWAY_SERVICE_NAME));
        bcos::gateway::GatewayInterface::Ptr gateway =
            std::make_shared<GatewayServiceClient>(gateWayProxy);

        // pbft
        auto pbftProxy = Application::getCommunicator()->stringToProxy<PBFTServicePrx>(
            getProxyDesc(PBFT_SERVICE_NAME));
        auto pbft = std::make_shared<PBFTServiceClient>(pbftProxy);
        auto sync = std::make_shared<BlockSyncServiceClient>(pbftProxy);

        // txPool
        auto txPoolProxy = Application::getCommunicator()->stringToProxy<TxPoolServicePrx>(
            getProxyDesc(TXPOOL_SERVICE_NAME));
        auto txpool = std::make_shared<bcostars::TxPoolServiceClient>(
            txPoolProxy, protocolInitializer->cryptoSuite());

        // executor
        auto executorProxy = Application::getCommunicator()->stringToProxy<ExecutorServicePrx>(
            getProxyDesc(EXECUTOR_SERVICE_NAME));
        auto executor = std::make_shared<bcostars::ExecutorServiceClient>(
            executorProxy, protocolInitializer->cryptoSuite());

        // storage
        auto storageProxy =
            Application::getCommunicator()->stringToProxy<bcostars::StorageServicePrx>(
                getProxyDesc(STORAGE_SERVICE_NAME));
        bcos::storage::StorageInterface::Ptr storageServiceClient =
            std::make_shared<bcostars::StorageServiceClient>(storageProxy);

        auto ledger = initLedger(protocolInitializer);
        auto factory = std::make_shared<bcos::rpc::RpcFactory>();
        // transaction factory
        factory->setGatewayInterface(gateway);
        factory->setBlockSyncInterface(sync);
        factory->setConsensusInterface(pbft);
        factory->setTxPoolInterface(txpool);
        factory->setExecutorInterface(executor);
        factory->setLedger(ledger);
        factory->setTransactionFactory(protocolInitializer->blockFactory()->transactionFactory());

        RPCSERVICE_LOG(INFO) << LOG_DESC("create rpc factory success");
        return factory;
    }

    void destroy() override
    {
        if (!m_running)
        {
            RPCSERVICE_LOG(WARNING) << LOG_DESC("The RpcService has already stopped!");
            return;
        }
        m_running = false;
        RPCSERVICE_LOG(INFO) << LOG_DESC("Stop the RpcService");

        if (m_rpcInterface)
        {
            m_rpcInterface->stop();
        }

        if (m_logInitializer)
        {
            m_logInitializer->stopLogging();
        }

        TLOGINFO(LOG_DESC("[RpcService] Stop the RpcService success") << std::endl);
    }

    virtual bcostars::Error asyncNotifyBlockNumber(
        tars::Int64 blockNumber, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_rpcInterface->asyncNotifyBlockNumber(blockNumber, [current](bcos::Error::Ptr _error) {
            async_response_asyncNotifyBlockNumber(current, toTarsError(_error));
        });
    }

private:
    static std::once_flag m_initFlag;
    static bcos::rpc::RPCInterface::Ptr m_rpcInterface;
    static bcos::BoostLogInitializer::Ptr m_logInitializer;
    static std::atomic_bool m_running;
};
}  // namespace bcostars