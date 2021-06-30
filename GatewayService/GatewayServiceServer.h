#pragma once

#include "../Common/ErrorConverter.h"
#include "GatewayService.h"
#include "bcos-crypto/signature/key/KeyFactoryImpl.h"
#include "bcos-framework/interfaces/crypto/KeyInterface.h"
#include "bcos-gateway/Gateway.h"
#include "bcos-gateway/GatewayFactory.h"
#include <mutex>

namespace bcostars
{
class GatewayServiceServer : public bcostars::GatewayService
{
public:
    void initialize() override
    {
        std::call_once(m_initFlag, []() {
            // bcos::gateway::Gat
            bcos::gateway::GatewayFactory factory;

            // TODO: add config path
            m_gateway = factory.buildGateway("");
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