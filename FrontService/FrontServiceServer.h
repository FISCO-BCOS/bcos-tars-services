#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/TarsUtils.h"
#include "../GatewayService/GatewayServiceClient.h"
#include "../PBFTService/PBFTServiceClient.h"
#include "../RpcService/RpcServiceClient.h"
#include "../TxPoolService/TxPoolServiceClient.h"
#include "../libinitializer/ProtocolInitializer.h"
#include "FrontService.h"
#include "libutilities/Common.h"
#include "libutilities/Log.h"
#include "servant/Communicator.h"
#include "servant/Global.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-framework/interfaces/crypto/KeyFactory.h>
#include <bcos-framework/interfaces/crypto/KeyInterface.h>
#include <bcos-framework/interfaces/front/FrontServiceInterface.h>
#include <bcos-framework/interfaces/protocol/Protocol.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-front/FrontService.h>
#include <bcos-front/FrontServiceFactory.h>
#include <boost/core/ignore_unused.hpp>

#define FRONTSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[FRONTSERVICE]"

namespace bcostars
{
class FrontServiceServer : public FrontService
{
public:
    ~FrontServiceServer() override {}
    void initialize() override
    {
        try
        {
            init();
            m_running = true;
        }
        catch (std::exception const& e)
        {
            TLOGERROR("init the FrontService exceptioned"
                      << LOG_KV("error", boost::diagnostic_information(e)) << std::endl);
            exit(0);
        }
    }

    void init()
    {
        std::call_once(m_onceFlag, [this]() {
            bcos::front::FrontServiceFactory frontServiceFactory;
            // load the config
            auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>();
            auto iniConfigPath = ServerConfig::BasePath + "config.ini";
            boost::property_tree::ptree pt;
            boost::property_tree::read_ini(iniConfigPath, pt);
            m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
            // set the boost log into the tars log directory
            m_logInitializer->setLogPath(getLogPath());
            m_logInitializer->initLog(pt);
            TLOGINFO(LOG_DESC("FrontService initLog success") << std::endl);

            nodeConfig->loadConfig(iniConfigPath);

            auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
            protocolInitializer->init(nodeConfig);
            auto privateKeyPath = ServerConfig::BasePath + "node.pem";
            protocolInitializer->loadKeyPair(privateKeyPath);
            m_keyFactory = protocolInitializer->keyFactory();
            FRONTSERVICE_LOG(INFO) << LOG_DESC("init crypto protocol success");

            // set the gateway interface
            auto gateWayProxy = Application::getCommunicator()->stringToProxy<GatewayServicePrx>(
                getProxyDesc(GATEWAY_SERVICE_NAME));
            auto gateWay = std::make_shared<GatewayServiceClient>(gateWayProxy, m_keyFactory);
            frontServiceFactory.setGatewayInterface(gateWay);
            FRONTSERVICE_LOG(INFO) << LOG_DESC("init the gateway client success");

            auto front = frontServiceFactory.buildFrontService(
                nodeConfig->groupId(), protocolInitializer->keyPair()->publicKey());
            FRONTSERVICE_LOG(INFO) << LOG_DESC("create the frontService success");

            // register the message dispatcher handler to the frontService
            auto pbftProxy = Application::getCommunicator()->stringToProxy<PBFTServicePrx>(
                getProxyDesc(PBFT_SERVICE_NAME));
            auto pbft = std::make_shared<PBFTServiceClient>(pbftProxy);
            // register the message dispatcher for PBFT module
            front->registerModuleMessageDispatcher(bcos::protocol::ModuleID::PBFT,
                [pbft](bcos::crypto::NodeIDPtr _nodeID, const std::string& _id,
                    bcos::bytesConstRef _data) {
                    pbft->asyncNotifyConsensusMessage(
                        nullptr, _id, _nodeID, _data, [](bcos::Error::Ptr _error) {
                            if (_error)
                            {
                                FRONTSERVICE_LOG(WARNING)
                                    << LOG_DESC("registerModuleMessageDispatcher failed")
                                    << LOG_KV("code", _error->errorCode())
                                    << LOG_KV("msg", _error->errorMessage());
                            }
                        });
                });
            FRONTSERVICE_LOG(INFO)
                << LOG_DESC("registerModuleMessageDispatcher for the consensus module success");
            // register the message dispatcher for the txsSync module
            auto txpoolProxy =
                Application::getCommunicator()->stringToProxy<bcostars::TxPoolServicePrx>(
                    getProxyDesc(TXPOOL_SERVICE_NAME));
            auto txpoolClient = std::make_shared<bcostars::TxPoolServiceClient>(
                txpoolProxy, protocolInitializer->cryptoSuite());
            front->registerModuleMessageDispatcher(bcos::protocol::ModuleID::TxsSync,
                [txpoolClient](bcos::crypto::NodeIDPtr _nodeID, std::string const& _id,
                    bcos::bytesConstRef _data) {
                    txpoolClient->asyncNotifyTxsSyncMessage(
                        nullptr, _id, _nodeID, _data, [_id](bcos::Error::Ptr _error) {
                            if (_error)
                            {
                                FRONTSERVICE_LOG(WARNING)
                                    << LOG_DESC("asyncNotifyTxsSyncMessage failed")
                                    << LOG_KV("code", _error->errorCode())
                                    << LOG_KV("msg", _error->errorMessage());
                            }
                        });
                });
            FRONTSERVICE_LOG(INFO)
                << LOG_DESC("registerModuleMessageDispatcher for the txsSync module success");
            // register the message dispatcher for the block sync module
            auto blockSync = std::make_shared<BlockSyncServiceClient>(pbftProxy);
            front->registerModuleMessageDispatcher(bcos::protocol::ModuleID::BlockSync,
                [blockSync](bcos::crypto::NodeIDPtr _nodeID, std::string const& _id,
                    bcos::bytesConstRef _data) {
                    blockSync->asyncNotifyBlockSyncMessage(
                        nullptr, _id, _nodeID, _data, [_id, _nodeID](bcos::Error::Ptr _error) {
                            if (_error)
                            {
                                FRONTSERVICE_LOG(WARNING)
                                    << LOG_DESC("asyncNotifyBlockSyncMessage failed")
                                    << LOG_KV("peer", _nodeID->shortHex()) << LOG_KV("id", _id)
                                    << LOG_KV("code", _error->errorCode())
                                    << LOG_KV("msg", _error->errorMessage());
                            }
                        });
                });
            FRONTSERVICE_LOG(INFO)
                << LOG_DESC("registerModuleMessageDispatcher for the BlockSync module success");
            // register the GetNodeIDsDispatcher to the frontService
            front->registerModuleNodeIDsDispatcher(bcos::protocol::ModuleID::TxsSync,
                [txpoolClient](std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
                    bcos::front::ReceiveMsgFunc _receiveMsgCallback) {
                    auto nodeIdSet = bcos::crypto::NodeIDSet(_nodeIDs->begin(), _nodeIDs->end());
                    txpoolClient->notifyConnectedNodes(nodeIdSet, _receiveMsgCallback);
                    FRONTSERVICE_LOG(DEBUG) << LOG_DESC("TxPool: notifyConnectedNodes")
                                            << LOG_KV("connectedNodeSize", nodeIdSet.size());
                });

            FRONTSERVICE_LOG(INFO)
                << LOG_DESC("registerModuleNodeIDsDispatcher for the TxsSync module success");

            front->registerModuleNodeIDsDispatcher(bcos::protocol::ModuleID::BlockSync,
                [blockSync](std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
                    bcos::front::ReceiveMsgFunc _receiveMsgCallback) {
                    auto nodeIdSet = bcos::crypto::NodeIDSet(_nodeIDs->begin(), _nodeIDs->end());
                    blockSync->notifyConnectedNodes(nodeIdSet, _receiveMsgCallback);
                    FRONTSERVICE_LOG(DEBUG) << LOG_DESC("BlockSync: notifyConnectedNodes")
                                            << LOG_KV("connectedNodeSize", nodeIdSet.size());
                });
            FRONTSERVICE_LOG(INFO)
                << LOG_DESC("registerModuleNodeIDsDispatcher for the BlockSync module success");

            auto rpcServicePrx = Application::getCommunicator()->stringToProxy<RpcServicePrx>(
                getProxyDesc(RPC_SERVICE_NAME));
            auto rpcServiceClient = std::make_shared<RpcServiceClient>(rpcServicePrx);
            // register the message dispatcher for the amop module
            front->registerModuleMessageDispatcher(bcos::protocol::ModuleID::AMOP,
                [rpcServiceClient](bcos::crypto::NodeIDPtr _nodeID, std::string const& _id,
                    bcos::bytesConstRef _data) {
                    rpcServiceClient->asyncNotifyAmopMessage(
                        _nodeID, _id, _data, [_id, _nodeID](bcos::Error::Ptr _error) {
                            if (_error)
                            {
                                FRONTSERVICE_LOG(WARNING)
                                    << LOG_DESC("asyncNotifyAmopMessage failed")
                                    << LOG_KV("peer", _nodeID->shortHex()) << LOG_KV("id", _id)
                                    << LOG_KV("code", _error->errorCode())
                                    << LOG_KV("msg", _error->errorMessage());
                            }
                        });
                });
            FRONTSERVICE_LOG(INFO)
                << LOG_DESC("registerModuleMessageDispatcher for the AMOP module success");

            // register the GetNodeIDsDispatcher to the frontService
            front->registerModuleNodeIDsDispatcher(bcos::protocol::ModuleID::AMOP,
                [rpcServiceClient](std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
                    bcos::front::ReceiveMsgFunc _receiveMsgCallback) {
                    rpcServiceClient->asyncNotifyAmopNodeIDs(_nodeIDs, _receiveMsgCallback);
                    FRONTSERVICE_LOG(DEBUG)
                        << LOG_DESC("asyncNotifyAmopNodeIDs")
                        << LOG_KV("connectedNodeSize", _nodeIDs ? _nodeIDs->size() : 0);
                });

            FRONTSERVICE_LOG(INFO)
                << LOG_DESC("registerModuleNodeIDsDispatcher for the AMOP module success");

            m_front = front;
            // start the front service
            FRONTSERVICE_LOG(INFO) << LOG_DESC("start the frontService");
            m_front->start();
            FRONTSERVICE_LOG(INFO) << LOG_DESC("start the frontService success");
        });
    }

    void destroy() override
    {
        if (!m_running)
        {
            FRONTSERVICE_LOG(WARNING) << LOG_DESC("The FrontService has already been stopped");
            return;
        }
        FRONTSERVICE_LOG(INFO) << LOG_DESC("stop the FrontService");
        m_running = false;
        if (m_front)
        {
            m_front->stop();
        }
    }

    bcostars::Error asyncGetNodeIDs(
        vector<vector<tars::Char>>& nodeIDs, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_front->asyncGetNodeIDs([current](bcos::Error::Ptr _error,
                                     std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs) {
            // Note: the nodeIDs maybe null if no connections
            std::vector<std::vector<char>> tarsNodeIDs;
            if (!_nodeIDs)
            {
                async_response_asyncGetNodeIDs(current, toTarsError(_error), tarsNodeIDs);
                return;
            }
            tarsNodeIDs.reserve(_nodeIDs->size());
            for (auto const& it : *_nodeIDs)
            {
                auto nodeIDData = it->data();
                tarsNodeIDs.emplace_back(nodeIDData.begin(), nodeIDData.end());
            }
            async_response_asyncGetNodeIDs(current, toTarsError(_error), tarsNodeIDs);
        });

        return bcostars::Error();
    }

    void asyncSendBroadcastMessage(
        tars::Int32 moduleID, const vector<tars::Char>& data, tars::TarsCurrentPtr current) override
    {
        m_front->asyncSendBroadcastMessage(
            moduleID, bcos::bytesConstRef((bcos::byte*)data.data(), data.size()));
    }

    bcostars::Error asyncSendMessageByNodeID(tars::Int32 moduleID, const vector<tars::Char>& nodeID,
        const vector<tars::Char>& data, tars::UInt32 timeout, tars::Bool requireRespCallback,
        vector<tars::Char>& responseNodeID, vector<tars::Char>& responseData, std::string& seq,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosNodeID =
            m_keyFactory->createKey(bcos::bytesConstRef((bcos::byte*)nodeID.data(), nodeID.size()));
        if (requireRespCallback)
        {
            m_front->asyncSendMessageByNodeID(moduleID, bcosNodeID,
                bcos::bytesConstRef((bcos::byte*)data.data(), data.size()), timeout,
                [current](bcos::Error::Ptr _error, bcos::crypto::NodeIDPtr _nodeID,
                    bcos::bytesConstRef _data, const std::string& _id,
                    bcos::front::ResponseFunc _respFunc) {
                    boost::ignore_unused(_respFunc);
                    auto encodedNodeID = *_nodeID->encode();
                    async_response_asyncSendMessageByNodeID(current, toTarsError(_error),
                        std::vector<char>(encodedNodeID.begin(), encodedNodeID.end()),
                        std::vector<char>(_data.begin(), _data.end()), _id);
                });
        }
        else
        {
            m_front->asyncSendMessageByNodeID(moduleID, bcosNodeID,
                bcos::bytesConstRef((bcos::byte*)data.data(), data.size()), timeout, nullptr);

            // response directly
            bcos::bytesConstRef respData;
            async_response_asyncSendMessageByNodeID(current, toTarsError(nullptr),
                std::vector<char>(nodeID.begin(), nodeID.end()),
                std::vector<char>(respData.begin(), respData.end()), seq);
        }

        return bcostars::Error();
    }

    void asyncSendMessageByNodeIDs(tars::Int32 moduleID, const vector<vector<tars::Char>>& nodeIDs,
        const vector<tars::Char>& data, tars::TarsCurrentPtr current) override
    {
        std::vector<bcos::crypto::NodeIDPtr> bcosNodeIDs;
        bcosNodeIDs.reserve(nodeIDs.size());
        for (auto const& it : nodeIDs)
        {
            bcosNodeIDs.push_back(
                m_keyFactory->createKey(bcos::bytesConstRef((bcos::byte*)it.data(), it.size())));
        }

        m_front->asyncSendMessageByNodeIDs(
            moduleID, bcosNodeIDs, bcos::bytesConstRef((bcos::byte*)data.data(), data.size()));
    }

    bcostars::Error asyncSendResponse(const std::string& id, tars::Int32 moduleID,
        const vector<tars::Char>& nodeID, const vector<tars::Char>& data,
        tars::TarsCurrentPtr current) override
    {
        FRONTSERVICE_LOG(TRACE) << LOG_DESC("asyncSendResponse server") << LOG_KV("id", id);
        current->setResponse(false);
        m_front->asyncSendResponse(id, moduleID,
            m_keyFactory->createKey(bcos::bytesConstRef((bcos::byte*)nodeID.data(), nodeID.size())),
            bcos::bytesConstRef((bcos::byte*)data.data(), data.size()),
            [current](bcos::Error::Ptr error) {
                async_response_asyncSendResponse(current, toTarsError(error));
            });
        return bcostars::Error();
    }

    bcostars::Error onReceiveBroadcastMessage(const std::string& groupID,
        const vector<tars::Char>& nodeID, const vector<tars::Char>& data,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_front->onReceiveBroadcastMessage(groupID,
            m_keyFactory->createKey(bcos::bytesConstRef((bcos::byte*)nodeID.data(), nodeID.size())),
            bcos::bytesConstRef((bcos::byte*)data.data(), data.size()),
            [current](bcos::Error::Ptr error) {
                async_response_onReceiveBroadcastMessage(current, toTarsError(error));
            });

        return bcostars::Error();
    }

    bcostars::Error onReceiveMessage(const std::string& groupID, const vector<tars::Char>& nodeID,
        const vector<tars::Char>& data, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_front->onReceiveMessage(groupID,
            m_keyFactory->createKey(bcos::bytesConstRef((bcos::byte*)nodeID.data(), nodeID.size())),
            bcos::bytesConstRef((bcos::byte*)data.data(), data.size()),
            [current](bcos::Error::Ptr error) {
                async_response_onReceiveMessage(current, toTarsError(error));
            });

        return bcostars::Error();
    }

    bcostars::Error onReceivedNodeIDs(const std::string& groupID,
        const vector<vector<tars::Char>>& nodeIDs, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosNodeIDs = std::make_shared<std::vector<bcos::crypto::NodeIDPtr>>();
        bcosNodeIDs->reserve(nodeIDs.size());

        for (auto const& it : nodeIDs)
        {
            bcosNodeIDs->push_back(
                m_keyFactory->createKey(bcos::bytesConstRef((bcos::byte*)it.data(), it.size())));
        }

        m_front->onReceiveNodeIDs(groupID, bcosNodeIDs, [current](bcos::Error::Ptr error) {
            async_response_onReceivedNodeIDs(current, toTarsError(error));
        });

        return bcostars::Error();
    }

private:
    static std::once_flag m_onceFlag;
    static bcos::front::FrontServiceInterface::Ptr m_front;
    std::atomic_bool m_running = {false};
    static bcos::crypto::KeyFactory::Ptr m_keyFactory;
    static bcos::BoostLogInitializer::Ptr m_logInitializer;
};
}  // namespace bcostars