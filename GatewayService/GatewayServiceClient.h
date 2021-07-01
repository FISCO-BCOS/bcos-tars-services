#pragma once

#include "../Common/ErrorConverter.h"
#include "GatewayService.h"
#include "bcos-framework/interfaces/gateway/GatewayInterface.h"

namespace bcostars
{
class GatewayServiceClient : public bcos::gateway::GatewayInterface
{
public:
    GatewayServiceClient(bcostars::GatewayServicePrx _proxy) : m_proxy(_proxy) {}
    virtual ~GatewayServiceClient() {}
    void asyncSendMessageByNodeID(const std::string& _groupID, bcos::crypto::NodeIDPtr _srcNodeID,
        bcos::crypto::NodeIDPtr _dstNodeID, bcos::bytesConstRef _payload,
        bcos::gateway::ErrorRespFunc _errorRespFunc) override
    {
        class Callback : public bcostars::GatewayServicePrxCallback
        {
        public:
            Callback(bcos::gateway::ErrorRespFunc callback) : m_callback(callback) {}

            void callback_asyncSendMessageByNodeID(const bcostars::Error& ret) override
            {
                m_callback(toBcosError(ret));
            }
            void callback_asyncSendMessageByNodeID_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret));
            }

        private:
            bcos::gateway::ErrorRespFunc m_callback;
        };

        m_proxy->async_asyncSendMessageByNodeID(new Callback(_errorRespFunc), _groupID,
            _srcNodeID->data(), _dstNodeID->data(), _payload.toBytes());
    }

    void asyncSendMessageByNodeIDs(const std::string& _groupID, bcos::crypto::NodeIDPtr _srcNodeID,
        const bcos::crypto::NodeIDs& _dstNodeIDs, bcos::bytesConstRef _payload) override
    {
        std::vector<bcos::bytes> tarsNodeIDs;
        for (auto const& it : _dstNodeIDs)
        {
            tarsNodeIDs.emplace_back(it->data());
        }
        m_proxy->async_asyncSendMessageByNodeIDs(
            nullptr, _groupID, _srcNodeID->data(), tarsNodeIDs, _payload.toBytes());
    }

    void asyncSendBroadcastMessage(const std::string& _groupID, bcos::crypto::NodeIDPtr _srcNodeID,
        bcos::bytesConstRef _payload) override
    {
        m_proxy->async_asyncSendBroadcastMessage(
            nullptr, _groupID, _srcNodeID->data(), _payload.toBytes());
    }

protected:
    void start() override {}
    void stop() override {}

private:
    bcostars::GatewayServicePrx m_proxy;
};
}  // namespace bcostars