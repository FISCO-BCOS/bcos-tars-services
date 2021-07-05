#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/ProxyDesc.h"
#include "../FrontService/FrontServiceClient.h"
#include "../libinitializer/ProtocolInitializer.h"
#include "GatewayService.h"
#include "bcos-crypto/signature/key/KeyFactoryImpl.h"
#include "bcos-framework/interfaces/crypto/KeyInterface.h"
#include "bcos-gateway/Gateway.h"
#include "bcos-gateway/GatewayFactory.h"
#include <bcos-framework/libtool/NodeConfig.h>
#include <mutex>

namespace bcostars
{
class GatewayServiceServer : public bcostars::GatewayService
{
public:
    void initialize() override
    {
        std::call_once(m_initFlag, []() {
            bcos::gateway::GatewayFactory factory;
            auto configPath = ServerConfig::BasePath + "config.ini";
            m_gateway = factory.buildGateway(configPath);

            auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
            auto privateKeyPath = ServerConfig::BasePath + "node.pem";
            protocolInitializer->loadKeyPair(privateKeyPath);

            auto frontServiceProxy =
                Application::getCommunicator()->stringToProxy<bcostars::FrontServicePrx>(
                    getProxyDesc("FrontServiceObj"));
            auto frontService = std::make_shared<bcostars::FrontServiceClient>(
                frontServiceProxy, protocolInitializer->keyFactory());
            auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>();
            nodeConfig->loadConfig(configPath);
            m_gateway->registerFrontService(
                nodeConfig->groupId(), protocolInitializer->keyPair()->publicKey(), frontService);
            // start the gateway
            m_gateway->start();
        });

        m_keyFactory = std::make_shared<bcos::crypto::KeyFactoryImpl>();
    }

    void destroy() override {}

    bcostars::Error asyncSendBroadcastMessage(const std::string& groupID,
        const vector<tars::UInt8>& srcNodeID, const vector<tars::UInt8>& payload,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosNodeID = m_keyFactory->createKey(srcNodeID);
        m_gateway->asyncSendBroadcastMessage(groupID, bcosNodeID, bcos::ref(payload));

        async_response_asyncSendBroadcastMessage(current, toTarsError(nullptr));
    }

    bcostars::Error asyncSendMessageByNodeID(const std::string& groupID,
        const vector<tars::UInt8>& srcNodeID, const vector<tars::UInt8>& dstNodeID,
        const vector<tars::UInt8>& payload, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosSrcNodeID = m_keyFactory->createKey(srcNodeID);
        auto bcosDstNodeID = m_keyFactory->createKey(dstNodeID);

        m_gateway->asyncSendMessageByNodeID(groupID, bcosSrcNodeID, bcosDstNodeID,
            bcos::ref(payload), [current](bcos::Error::Ptr error) {
                async_response_asyncSendMessageByNodeID(current, toTarsError(error));
            });
    }

    bcostars::Error asyncSendMessageByNodeIDs(const std::string& groupID,
        const vector<tars::UInt8>& srcNodeID, const vector<vector<tars::UInt8>>& dstNodeID,
        const vector<tars::UInt8>& payload, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosSrcNodeID = m_keyFactory->createKey(srcNodeID);
        std::vector<bcos::crypto::NodeIDPtr> nodeIDs;
        nodeIDs.reserve(dstNodeID.size());
        for (auto const& it : dstNodeID)
        {
            nodeIDs.push_back(m_keyFactory->createKey(it));
        }

        m_gateway->asyncSendMessageByNodeIDs(groupID, bcosSrcNodeID, nodeIDs, bcos::ref(payload));

        async_response_asyncSendMessageByNodeIDs(current, toTarsError(nullptr));
    }

private:
    static std::once_flag m_initFlag;
    static bcos::gateway::Gateway::Ptr m_gateway;
    bcos::crypto::KeyFactory::Ptr m_keyFactory;
};
}  // namespace bcostars