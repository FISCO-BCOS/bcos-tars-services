#include "RpcServiceServer.h"
#include <bcos-framework/libutilities/Log.h>
#include <bcos-tars-protocol/client/GroupManagerServiceClient.h>
#include <memory>
using namespace bcostars;
using namespace bcos::group;

std::once_flag RpcServiceServer::m_initFlag;
bcos::rpc::Rpc::Ptr RpcServiceServer::m_rpc;
bcos::BoostLogInitializer::Ptr RpcServiceServer::m_logInitializer;
bcos::crypto::KeyFactory::Ptr RpcServiceServer::m_keyFactory;
std::atomic_bool RpcServiceServer::m_running = {false};

bcos::group::GroupInfoFactory::Ptr RpcServiceServer::m_groupInfoFactory;
bcos::group::ChainNodeInfoFactory::Ptr RpcServiceServer::m_chainNodeInfoFactory;

void RpcServiceServer::initialize()
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

void RpcServiceServer::destroy()
{
    if (!m_running)
    {
        RPCSERVICE_LOG(WARNING) << LOG_DESC("The RpcService has already stopped!");
        return;
    }
    m_running = false;
    RPCSERVICE_LOG(INFO) << LOG_DESC("Stop the RpcService");

    if (m_rpc)
    {
        m_rpc->stop();
    }
    TLOGINFO(LOG_DESC("[RpcService] Stop the RpcService success") << std::endl);
}

void RpcServiceServer::init()
{
    // load the configuration
    auto configPath = ServerConfig::BasePath + "config.ini";
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(configPath, pt);
    // init log
    m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
    m_logInitializer->setLogPath(getLogPath());
    m_logInitializer->initLog(pt);
    RPCSERVICE_LOG(INFO) << LOG_DESC("init log success");

    // init node config
    RPCSERVICE_LOG(INFO) << LOG_DESC("init node config");
    auto nodeConfig =
        std::make_shared<bcos::tool::NodeConfig>(std::make_shared<bcos::crypto::KeyFactoryImpl>());
    nodeConfig->loadConfig(configPath);
    RPCSERVICE_LOG(INFO) << LOG_DESC("init node config success");

    // init rpc config
    RPCSERVICE_LOG(INFO) << LOG_DESC("init rpc factory");
    auto factory = initRpcFactory(nodeConfig);
    RPCSERVICE_LOG(INFO) << LOG_DESC("init rpc factory success");
    auto config = factory->initConfig(configPath);
    RPCSERVICE_LOG(INFO) << LOG_DESC("init rpc config success");

    auto rpc = factory->buildRpc(config);
    m_rpc = rpc;
    RPCSERVICE_LOG(INFO) << LOG_DESC("start rpc");
    m_rpc->start();
    RPCSERVICE_LOG(INFO) << LOG_DESC("start rpc success");

    TLOGINFO(RPCSERVICE_BADGE << LOG_DESC("the rpc service success") << std::endl);
}

bcos::rpc::RpcFactory::Ptr RpcServiceServer::initRpcFactory(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    RPCSERVICE_LOG(INFO) << LOG_DESC("create rpc factory");

    // init the protocol
    RPCSERVICE_LOG(INFO) << LOG_DESC("init protocol");
    auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
    protocolInitializer->init(_nodeConfig);
    m_keyFactory = protocolInitializer->keyFactory();
    RPCSERVICE_LOG(INFO) << LOG_DESC("init protocol success");

    // get the gateway client
    auto gatewayProxy = Application::getCommunicator()->stringToProxy<GatewayServicePrx>(
        _nodeConfig->gatewayServiceName());
    auto gateway =
        std::make_shared<GatewayServiceClient>(gatewayProxy, protocolInitializer->keyFactory());

    // get the group manager service client
    m_groupInfoFactory = std::make_shared<GroupInfoFactory>();
    m_chainNodeInfoFactory = std::make_shared<ChainNodeInfoFactory>();
    auto groupManagerPrx = Application::getCommunicator()->stringToProxy<GroupManagerServicePrx>(
        _nodeConfig->groupManagerServiceName());
    auto groupManagerClient = std::make_shared<GroupManagerServiceClient>(
        groupManagerPrx, m_chainNodeInfoFactory, m_groupInfoFactory);
    auto factory =
        std::make_shared<bcos::rpc::RpcFactory>(_nodeConfig->chainId(), gateway, groupManagerClient,
            m_groupInfoFactory, m_chainNodeInfoFactory, protocolInitializer->keyFactory());
    RPCSERVICE_LOG(INFO) << LOG_DESC("create rpc factory success");
    return factory;
}

bcostars::Error RpcServiceServer::asyncNotifyBlockNumber(
    tars::Int64 blockNumber, tars::TarsCurrentPtr current)
{
    current->setResponse(false);

    m_rpc->asyncNotifyBlockNumber(blockNumber, [current, blockNumber](bcos::Error::Ptr _error) {
        RPCSERVICE_LOG(DEBUG) << LOG_BADGE("asyncNotifyBlockNumber")
                              << LOG_KV("blockNumber", blockNumber)
                              << LOG_KV("errorCode", _error ? _error->errorCode() : 0)
                              << LOG_KV("errorMessage", _error ? _error->errorMessage() : "");
        async_response_asyncNotifyBlockNumber(current, toTarsError(_error));
    });

    return bcostars::Error();
}

bcostars::Error RpcServiceServer::asyncNotifyAmopNodeIDs(
    const vector<vector<tars::Char> >& _nodeIDs, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    std::shared_ptr<bcos::crypto::NodeIDs> nodeIDs = std::make_shared<bcos::crypto::NodeIDs>();
    for (const auto& nodeID : _nodeIDs)
    {
        auto nodeIDPtr = m_keyFactory->createKey(
            bcos::bytesConstRef((const bcos::byte*)nodeID.data(), nodeID.size()));
        nodeIDs->push_back(nodeIDPtr);
    }

    m_rpc->asyncNotifyAmopNodeIDs(nodeIDs, [current, nodeIDs](bcos::Error::Ptr _error) {
        RPCSERVICE_LOG(DEBUG) << LOG_BADGE("asyncNotifyAmopNodeIDs")
                              << LOG_KV("nodeIDs size", nodeIDs->size())
                              << LOG_KV("errorCode", _error ? _error->errorCode() : 0)
                              << LOG_KV("errorMessage", _error ? _error->errorMessage() : "");
        async_response_asyncNotifyAmopNodeIDs(current, toTarsError(_error));
    });

    return bcostars::Error();
}

bcostars::Error RpcServiceServer::asyncNotifyAmopMessage(const vector<tars::Char>& _nodeID,
    const std::string& _uuid, const vector<tars::Char>& _data, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    auto nodeIDPtr = m_keyFactory->createKey(
        bcos::bytesConstRef((const bcos::byte*)_nodeID.data(), _nodeID.size()));
    m_rpc->asyncNotifyAmopMessage(nodeIDPtr, _uuid,
        bcos::bytesConstRef((bcos::byte*)_data.data(), _data.size()),
        [current, nodeIDPtr](bcos::Error::Ptr _error) {
            RPCSERVICE_LOG(TRACE) << LOG_BADGE("asyncNotifyAmopMessage")
                                  << LOG_KV("nodeID", nodeIDPtr->hex())
                                  << LOG_KV("errorCode", _error ? _error->errorCode() : 0)
                                  << LOG_KV("errorMessage", _error ? _error->errorMessage() : "");
            async_response_asyncNotifyAmopMessage(current, toTarsError(_error));
        });

    return bcostars::Error();
}

bcostars::Error RpcServiceServer::asyncNotifyGroupInfo(
    const bcostars::GroupInfo& groupInfo, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    auto bcosGroupInfo = toBcosGroupInfo(m_chainNodeInfoFactory, m_groupInfoFactory, groupInfo);
    m_rpc->asyncNotifyGroupInfo(bcosGroupInfo, [current](bcos::Error::Ptr&& _error) {
        async_response_asyncNotifyGroupInfo(current, toTarsError(_error));
    });
    return bcostars::Error();
}
