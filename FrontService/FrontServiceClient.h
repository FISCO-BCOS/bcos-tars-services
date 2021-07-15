#pragma once

#include "../Common/ErrorConverter.h"
#include "FrontService.h"
#include "bcos-framework/interfaces/crypto/KeyFactory.h"
#include "bcos-framework/interfaces/front/FrontServiceInterface.h"
#include "bcos-framework/libutilities/RefDataContainer.h"

namespace bcostars
{
class FrontServiceClient : public bcos::front::FrontServiceInterface
{
public:
    void start() override {}
    void stop() override {}

    FrontServiceClient(bcostars::FrontServicePrx proxy, bcos::crypto::KeyFactory::Ptr keyFactory)
      : m_proxy(proxy), m_keyFactory(keyFactory)
    {}

    void asyncGetNodeIDs(bcos::front::GetNodeIDsFunc _getNodeIDsFunc) override
    {
        class Callback : public FrontServicePrxCallback
        {
        public:
            Callback(bcos::front::GetNodeIDsFunc callback, FrontServiceClient* self)
              : m_callback(callback), m_self(self)
            {}
            void callback_asyncGetNodeIDs(
                const bcostars::Error& ret, const vector<vector<tars::UInt8>>& nodeIDs) override
            {
                auto bcosNodeIDs = std::make_shared<std::vector<bcos::crypto::NodeIDPtr>>();
                bcosNodeIDs->reserve(nodeIDs.size());
                for (auto const& it : nodeIDs)
                {
                    bcosNodeIDs->push_back(m_self->m_keyFactory->createKey(it));
                }

                m_callback(toBcosError(ret), bcosNodeIDs);
            }
            void callback_asyncGetNodeIDs_exception(tars::Int32 ret) override
            {
                m_callback(toBcosError(ret), nullptr);
            }

        private:
            bcos::front::GetNodeIDsFunc m_callback;
            FrontServiceClient* m_self;
        };

        m_proxy->async_asyncGetNodeIDs(new Callback(_getNodeIDsFunc, this));
    }

    void onReceiveNodeIDs(const std::string& _groupID,
        std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
        bcos::front::ReceiveMsgFunc _receiveMsgCallback) override
    {
        class Callback : public FrontServicePrxCallback
        {
        public:
            Callback(bcos::front::ReceiveMsgFunc callback) : m_callback(callback) {}

            void callback_onReceivedNodeIDs(const bcostars::Error& ret) override
            {
                if (!m_callback)
                {
                    return;
                }
                m_callback(toBcosError(ret));
            }

            void callback_onReceivedNodeIDs_exception(tars::Int32 ret) override
            {
                if (!m_callback)
                {
                    return;
                }
                m_callback(toBcosError(ret));
            }

        private:
            bcos::front::ReceiveMsgFunc m_callback;
        };

        std::vector<bcos::bytes> tarsNodeIDs;
        tarsNodeIDs.reserve(_nodeIDs->size());
        for (auto const& it : *_nodeIDs)
        {
            tarsNodeIDs.push_back(it->data());
        }

        m_proxy->async_onReceivedNodeIDs(new Callback(_receiveMsgCallback), _groupID, tarsNodeIDs);
    }

    void onReceiveMessage(const std::string& _groupID, bcos::crypto::NodeIDPtr _nodeID,
        bcos::bytesConstRef _data, bcos::front::ReceiveMsgFunc _receiveMsgCallback) override
    {
        class Callback : public FrontServicePrxCallback
        {
        public:
            Callback(bcos::front::ReceiveMsgFunc callback) : m_callback(callback) {}

            void callback_onReceiveMessage(const bcostars::Error& ret) override
            {
                if (!m_callback)
                {
                    return;
                }
                m_callback(toBcosError(ret));
            }

            void callback_onReceiveMessage_exception(tars::Int32 ret) override
            {
                if (!m_callback)
                {
                    return;
                }
                m_callback(toBcosError(ret));
            }

        private:
            bcos::front::ReceiveMsgFunc m_callback;
        };

        m_proxy->async_onReceiveMessage(
            new Callback(_receiveMsgCallback), _groupID, _nodeID->data(), _data.toBytes());
    }

    // Note: the _receiveMsgCallback maybe null in some cases
    void onReceiveBroadcastMessage(const std::string& _groupID, bcos::crypto::NodeIDPtr _nodeID,
        bcos::bytesConstRef _data, bcos::front::ReceiveMsgFunc _receiveMsgCallback) override
    {
        class Callback : public FrontServicePrxCallback
        {
        public:
            Callback(bcos::front::ReceiveMsgFunc callback) : m_callback(callback) {}

            void callback_onReceiveBroadcastMessage(const bcostars::Error& ret) override
            {
                if (!m_callback)
                {
                    return;
                }
                m_callback(toBcosError(ret));
            }

            void callback_onReceiveBroadcastMessage_exception(tars::Int32 ret) override
            {
                if (!m_callback)
                {
                    return;
                }
                m_callback(toBcosError(ret));
            }

        private:
            bcos::front::ReceiveMsgFunc m_callback;
        };

        m_proxy->async_onReceiveBroadcastMessage(
            new Callback(_receiveMsgCallback), _groupID, _nodeID->data(), _data.toBytes());
    }

    // Note: the _callback maybe null in some cases
    void asyncSendMessageByNodeID(int _moduleID, bcos::crypto::NodeIDPtr _nodeID,
        bcos::bytesConstRef _data, uint32_t _timeout, bcos::front::CallbackFunc _callback) override
    {
        class Callback : public FrontServicePrxCallback
        {
        public:
            Callback(bcos::front::CallbackFunc callback, FrontServiceClient* self)
              : m_callback(callback), m_self(self)
            {}

            void callback_asyncSendMessageByNodeID(const bcostars::Error& ret,
                const vector<tars::UInt8>& responseNodeID, const vector<tars::UInt8>& responseData,
                const std::string& seq) override
            {
                if (!m_callback)
                {
                    return;
                }
                auto bcosNodeID = m_self->m_keyFactory->createKey(responseNodeID);
                m_callback(toBcosError(ret), bcosNodeID, bcos::ref(responseData), seq,
                    bcos::front::ResponseFunc());
            }

            void callback_asyncSendMessageByNodeID_exception(tars::Int32 ret) override
            {
                if (!m_callback)
                {
                    return;
                }
                m_callback(toBcosError(ret), nullptr, bcos::bytesConstRef(), "",
                    bcos::front::ResponseFunc());
            }

        private:
            bcos::front::CallbackFunc m_callback;
            FrontServiceClient* m_self;
        };

        m_proxy->async_asyncSendMessageByNodeID(
            new Callback(_callback, this), _moduleID, _nodeID->data(), _data.toBytes(), _timeout);
    }

    void asyncSendResponse(const std::string& _id, int _moduleID, bcos::crypto::NodeIDPtr _nodeID,
        bcos::bytesConstRef _data, bcos::front::ReceiveMsgFunc _receiveMsgCallback) override
    {
        m_proxy->asyncSendResponse(_id, _moduleID, _nodeID->data(), _data.toBytes());
    }

    void asyncSendMessageByNodeIDs(int _moduleID,
        const std::vector<bcos::crypto::NodeIDPtr>& _nodeIDs, bcos::bytesConstRef _data) override
    {
        std::vector<bcos::bytes> tarsNodeIDs;
        tarsNodeIDs.reserve(_nodeIDs.size());
        for (auto const& it : _nodeIDs)
        {
            tarsNodeIDs.push_back(it->data());
        }
        m_proxy->async_asyncSendMessageByNodeIDs(nullptr, _moduleID, tarsNodeIDs, _data.toBytes());
    }

    void asyncSendBroadcastMessage(int _moduleID, bcos::bytesConstRef _data) override
    {
        m_proxy->async_asyncSendBroadcastMessage(nullptr, _moduleID, _data.toBytes());
    }

private:
    bcostars::FrontServicePrx m_proxy;
    bcos::crypto::KeyFactory::Ptr m_keyFactory;
};
}  // namespace bcostars