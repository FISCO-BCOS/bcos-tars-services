#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/ProxyDesc.h"
#include "../GatewayService/GatewayServiceClient.h"
#include "../PBFTService/PBFTServiceClient.h"
#include "../TxPoolService/TxPoolServiceClient.h"
#include "../libinitializer/ProtocolInitializer.h"
#include "FrontService.h"
#include "bcos-framework/interfaces/crypto/KeyFactory.h"
#include "bcos-framework/interfaces/crypto/KeyInterface.h"
#include "bcos-framework/interfaces/front/FrontServiceInterface.h"
#include "servant/Communicator.h"
#include "servant/Global.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-framework/interfaces/protocol/Protocol.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-front/FrontService.h>
#include <bcos-front/FrontServiceFactory.h>

#define FRONTSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[FRONTSERVICE]"

namespace bcostars
{
class FrontServiceServer : public FrontService
{
    ~FrontServiceServer() override {}

    void initialize() override
    {
        std::call_once(m_onceFlag, [this]() {
            bcos::front::FrontServiceFactory frontServiceFactory;
            // load the config
            auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>();
            auto iniConfigPath = ServerConfig::BasePath + "config.ini";
            nodeConfig->loadConfig(iniConfigPath);

            auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
            auto privateKeyPath = ServerConfig::BasePath + "node.pem";
            protocolInitializer->loadKeyPair(privateKeyPath);
            m_keyFactory = protocolInitializer->keyFactory();

            // set the gateway interface
            auto gateWayProxy = Application::getCommunicator()->stringToProxy<GatewayServicePrx>(
                getProxyDesc("GatewayServiceObj"));
            auto gateWay = std::make_shared<GatewayServiceClient>(gateWayProxy);
            frontServiceFactory.setGatewayInterface(gateWay);

            auto front = frontServiceFactory.buildFrontService(
                nodeConfig->groupId(), protocolInitializer->keyPair()->publicKey());
            // register the message dispatcher handler to the frontService
            auto pbftProxy = Application::getCommunicator()->stringToProxy<PBFTServicePrx>(
                getProxyDesc("PBFTServiceObj"));
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
            // register the message dispatcher for the txsSync module
            auto txpoolProxy =
                Application::getCommunicator()->stringToProxy<bcostars::TxPoolServicePrx>(
                    getProxyDesc("TxPoolServiceObj"));
            auto txpoolClient = std::make_shared<bcostars::TxPoolServiceClient>(
                txpoolProxy, protocolInitializer->cryptoSuite());
            front->registerModuleMessageDispatcher(bcos::protocol::ModuleID::TxsSync,
                [txpoolClient](bcos::crypto::NodeIDPtr _nodeID, std::string const& _id,
                    bcos::bytesConstRef _data) {
                    txpoolClient->asyncNotifyTxsSyncMessage(
                        nullptr, _id, _nodeID, _data, [](bcos::Error::Ptr _error) {
                            if (_error)
                            {
                                FRONTSERVICE_LOG(WARNING)
                                    << LOG_DESC("asyncNotifyTxsSyncMessage failed")
                                    << LOG_KV("code", _error->errorCode())
                                    << LOG_KV("msg", _error->errorMessage());
                            }
                        });
                });

            // register the message dispatcher for the block sync module
            auto blockSync = std::make_shared<BlockSyncServiceClient>(pbftProxy);
            front->registerModuleMessageDispatcher(bcos::protocol::ModuleID::BlockSync,
                [blockSync](bcos::crypto::NodeIDPtr _nodeID, std::string const& _id,
                    bcos::bytesConstRef _data) {
                    blockSync->asyncNotifyBlockSyncMessage(
                        nullptr, _id, _nodeID, _data, [](bcos::Error::Ptr _error) {
                            if (_error)
                            {
                                FRONTSERVICE_LOG(WARNING)
                                    << LOG_DESC("asyncNotifyBlockSyncMessage failed")
                                    << LOG_KV("code", _error->errorCode())
                                    << LOG_KV("msg", _error->errorMessage());
                            }
                        });
                });
            m_front = front;
            // start the front service
            m_front->start();
        });
    }

    void destroy() override {}

    bcostars::Error asyncGetNodeIDs(
        vector<vector<tars::UInt8>>& nodeIDs, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_front->asyncGetNodeIDs([current](bcos::Error::Ptr _error,
                                     std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs) {
            std::vector<bcos::bytes> tarsNodeIDs;
            tarsNodeIDs.reserve(_nodeIDs->size());
            for (auto const& it : *_nodeIDs)
            {
                tarsNodeIDs.push_back(it->data());
            }

            async_response_asyncGetNodeIDs(current, toTarsError(_error), tarsNodeIDs);
        });

        return bcostars::Error();
    }

    void asyncSendBroadcastMessage(tars::Int32 moduleID, const vector<tars::UInt8>& data,
        tars::TarsCurrentPtr current) override
    {
        m_front->asyncSendBroadcastMessage(moduleID, bcos::ref(data));
    }

    bcostars::Error asyncSendMessageByNodeID(tars::Int32 moduleID,
        const vector<tars::UInt8>& nodeID, const vector<tars::UInt8>& data, tars::UInt32 timeout,
        vector<tars::UInt8>& responseNodeID, vector<tars::UInt8>& responseData, std::string& seq,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosNodeID = m_keyFactory->createKey(nodeID);
        m_front->asyncSendMessageByNodeID(moduleID, bcosNodeID, bcos::ref(data), timeout,
            [current](bcos::Error::Ptr _error, bcos::crypto::NodeIDPtr _nodeID,
                bcos::bytesConstRef _data, const std::string& _id,
                bcos::front::ResponseFunc _respFunc) {
                async_response_asyncSendMessageByNodeID(
                    current, toTarsError(_error), *_nodeID->encode(), _data.toBytes(), _id);
            });

        return bcostars::Error();
    }

    void asyncSendMessageByNodeIDs(tars::Int32 moduleID, const vector<vector<tars::UInt8>>& nodeIDs,
        const vector<tars::UInt8>& data, tars::TarsCurrentPtr current) override
    {
        std::vector<bcos::crypto::NodeIDPtr> bcosNodeIDs;
        bcosNodeIDs.reserve(nodeIDs.size());
        for (auto const& it : nodeIDs)
        {
            bcosNodeIDs.push_back(m_keyFactory->createKey(it));
        }

        m_front->asyncSendMessageByNodeIDs(moduleID, bcosNodeIDs, bcos::ref(data));
    }

    bcostars::Error asyncSendResponse(const std::string& id, tars::Int32 moduleID,
        const vector<tars::UInt8>& nodeID, const vector<tars::UInt8>& data,
        tars::TarsCurrentPtr current) override
    {
        m_front->asyncSendResponse(id, moduleID, m_keyFactory->createKey(nodeID), bcos::ref(data),
            [current](bcos::Error::Ptr error) {
                async_response_asyncSendResponse(current, toTarsError(error));
            });
    }

    bcostars::Error onReceiveBroadcastMessage(const std::string& groupID,
        const vector<tars::UInt8>& nodeID, const vector<tars::UInt8>& data,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_front->onReceiveBroadcastMessage(groupID, m_keyFactory->createKey(nodeID),
            bcos::ref(data), [current](bcos::Error::Ptr error) {
                async_response_onReceiveBroadcastMessage(current, toTarsError(error));
            });

        return bcostars::Error();
    }

    bcostars::Error onReceiveMessage(const std::string& groupID, const vector<tars::UInt8>& nodeID,
        const vector<tars::UInt8>& data, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        m_front->onReceiveMessage(groupID, m_keyFactory->createKey(nodeID), bcos::ref(data),
            [current](bcos::Error::Ptr error) {
                async_response_onReceiveMessage(current, toTarsError(error));
            });

        return bcostars::Error();
    }

    bcostars::Error onReceivedNodeIDs(const std::string& groupID,
        const vector<vector<tars::UInt8>>& nodeIDs, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosNodeIDs = std::make_shared<std::vector<bcos::crypto::NodeIDPtr>>();
        bcosNodeIDs->reserve(nodeIDs.size());

        for (auto const& it : nodeIDs)
        {
            bcosNodeIDs->push_back(m_keyFactory->createKey(it));
        }

        m_front->onReceiveNodeIDs(groupID, bcosNodeIDs, [current](bcos::Error::Ptr error) {
            async_response_onReceivedNodeIDs(current, toTarsError(error));
        });

        return bcostars::Error();
    }

private:
    static std::once_flag m_onceFlag;
    static bcos::front::FrontServiceInterface::Ptr m_front;
    bcos::crypto::KeyFactory::Ptr m_keyFactory;
};
}  // namespace bcostars