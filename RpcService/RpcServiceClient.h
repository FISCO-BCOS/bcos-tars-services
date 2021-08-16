#pragma once

#include "../Common/ErrorConverter.h"
#include "RpcService.h"
#include <bcos-framework/interfaces/amop/AMOPInterface.h>
#include <bcos-framework/interfaces/rpc/RPCInterface.h>
#include <bcos-framework/libutilities/Common.h>

namespace bcostars
{
class RpcServiceClient : public bcos::rpc::RPCInterface, bcos::amop::AMOPInterface
{
public:
    RpcServiceClient(bcostars::RpcServicePrx _proxy) : m_proxy(_proxy) {}
    ~RpcServiceClient() override {}

    class Callback : public RpcServicePrxCallback
    {
    public:
        explicit Callback(std::function<void(bcos::Error::Ptr)> _callback)
          : RpcServicePrxCallback(), m_callback(_callback)
        {}
        ~Callback() override {}

        virtual void callback_asyncNotifyBlockNumber(const bcostars::Error& ret) override
        {
            m_callback(toBcosError(ret));
        }
        virtual void callback_asyncNotifyBlockNumber_exception(tars::Int32 ret) override
        {
            m_callback(toBcosError(ret));
        }

        virtual void callback_asyncNotifyAmopMessage(const bcostars::Error& ret) override
        {
            m_callback(toBcosError(ret));
        }
        virtual void callback_asyncNotifyAmopMessage_exception(tars::Int32 ret) override
        {
            m_callback(toBcosError(ret));
        }

        virtual void callback_asyncNotifyAmopNodeIDs(const bcostars::Error& ret) override
        {
            m_callback(toBcosError(ret));
        }
        virtual void callback_asyncNotifyAmopNodeIDs_exception(tars::Int32 ret) override
        {
            m_callback(toBcosError(ret));
        }

    private:
        std::function<void(bcos::Error::Ptr)> m_callback;
    };

    void asyncNotifyBlockNumber(bcos::protocol::BlockNumber _blockNumber,
        std::function<void(bcos::Error::Ptr)> _callback) override
    {
        m_proxy->async_asyncNotifyBlockNumber(new Callback(_callback), _blockNumber);
    }

    void asyncNotifyAmopMessage(bcos::crypto::NodeIDPtr _nodeID, const std::string& _id,
        bcos::bytesConstRef _data, std::function<void(bcos::Error::Ptr _error)> _callback) override
    {
        auto encodedNodeID = *_nodeID->encode();
        m_proxy->async_asyncNotifyAmopMessage(new Callback(_callback),
            std::vector<char>(encodedNodeID.begin(), encodedNodeID.end()), _id,
            std::vector<char>(_data.begin(), _data.end()));
    }

    void asyncNotifyAmopNodeIDs(std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
        std::function<void(bcos::Error::Ptr _error)> _callback) override
    {
        vector<vector<tars::Char>> encodedNodeIDs;
        if (_nodeIDs && !_nodeIDs->empty())
        {
            for (const auto& nodeID : *_nodeIDs)
            {
                auto encodedNodeID = *nodeID->encode();
                encodedNodeIDs.push_back(
                    std::vector<char>(encodedNodeID.begin(), encodedNodeID.end()));
            }
        }
        m_proxy->async_asyncNotifyAmopNodeIDs(new Callback(_callback), encodedNodeIDs);
    }

protected:
    void start() override {}
    void stop() override {}

private:
    bcostars::RpcServicePrx m_proxy;
};

}  // namespace bcostars