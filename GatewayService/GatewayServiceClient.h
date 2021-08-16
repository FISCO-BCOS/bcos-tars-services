#pragma once

#include "../Common/ErrorConverter.h"
#include "GatewayService.h"
#include "bcos-framework/interfaces/gateway/GatewayInterface.h"
#include "servant/RemoteLogger.h"
#include <string>

#define GATEWAYCLIENT_LOG(LEVEL) BCOS_LOG(LEVEL) << "[GATEWAYCLIENT][INITIALIZER]"
#define GATEWAYCLIENT_BADGE "[GATEWAYCLIENT]"
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

        auto srcNodeID = _srcNodeID->data();
        auto destNodeID = _dstNodeID->data();
        m_proxy->async_asyncSendMessageByNodeID(new Callback(_errorRespFunc), _groupID,
            std::vector<char>(srcNodeID.begin(), srcNodeID.end()),
            std::vector<char>(destNodeID.begin(), destNodeID.end()),
            std::vector<char>(_payload.begin(), _payload.end()));
    }

    void asyncGetPeers(bcos::gateway::PeerRespFunc _peerRespFunc) override
    {
        class Callback : public bcostars::GatewayServicePrxCallback
        {
        public:
            Callback(bcos::gateway::PeerRespFunc callback) : m_callback(callback) {}

            void callback_asyncGetPeers(
                const bcostars::Error& ret, const std::string& _peers) override
            {
                m_callback(toBcosError(ret), _peers);
            }
            void callback_asyncGetPeers_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), "");
            }

        private:
            bcos::gateway::PeerRespFunc m_callback;
        };

        auto t1 = std::chrono::high_resolution_clock::now();
        GATEWAYCLIENT_LOG(DEBUG) << LOG_BADGE("asyncGetPeers") << LOG_DESC("request");
        m_proxy->async_asyncGetPeers(
            new Callback([_peerRespFunc, t1](bcos::Error::Ptr _error, const std::string& _peers) {
                auto t2 = std::chrono::high_resolution_clock::now();
                GATEWAYCLIENT_LOG(DEBUG) << LOG_BADGE("asyncGetPeers") << LOG_KV("response", _peers)
                                         << LOG_KV("cost", (t2 - t1).count());
                _peerRespFunc(_error, _peers);
            }));
    }

    void asyncSendMessageByNodeIDs(const std::string& _groupID, bcos::crypto::NodeIDPtr _srcNodeID,
        const bcos::crypto::NodeIDs& _dstNodeIDs, bcos::bytesConstRef _payload) override
    {
        std::vector<std::vector<char>> tarsNodeIDs;
        for (auto const& it : _dstNodeIDs)
        {
            auto nodeID = it->data();
            tarsNodeIDs.emplace_back(nodeID.begin(), nodeID.end());
        }

        auto srcNodeID = _srcNodeID->data();
        m_proxy->async_asyncSendMessageByNodeIDs(nullptr, _groupID,
            std::vector<char>(srcNodeID.begin(), srcNodeID.end()), tarsNodeIDs,
            std::vector<char>(_payload.begin(), _payload.end()));
    }

    void asyncSendBroadcastMessage(const std::string& _groupID, bcos::crypto::NodeIDPtr _srcNodeID,
        bcos::bytesConstRef _payload) override
    {
        auto srcNodeID = _srcNodeID->data();
        m_proxy->async_asyncSendBroadcastMessage(nullptr, _groupID,
            std::vector<char>(srcNodeID.begin(), srcNodeID.end()),
            std::vector<char>(_payload.begin(), _payload.end()));
    }

protected:
    void start() override {}
    void stop() override {}

private:
    bcostars::GatewayServicePrx m_proxy;
};
}  // namespace bcostars