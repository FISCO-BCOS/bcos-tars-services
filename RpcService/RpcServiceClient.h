#pragma once

#include "../Common/ErrorConverter.h"
#include "RpcService.h"
#include "libutilities/Common.h"
#include <bcos-framework/interfaces/rpc/RPCInterface.h>

namespace bcostars
{
class RpcServiceClient : public bcos::rpc::RPCInterface
{
public:
    RpcServiceClient(bcostars::RpcServicePrx _proxy) : m_proxy(_proxy) {}
    ~RpcServiceClient() override {}

    void asyncNotifyBlockNumber(bcos::protocol::BlockNumber _blockNumber,
        std::function<void(bcos::Error::Ptr)> _callback) override
    {
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

        private:
            std::function<void(bcos::Error::Ptr)> m_callback;
        };

        m_proxy->async_asyncNotifyBlockNumber(new Callback(_callback), _blockNumber);
    }

protected:
    void start() override {}
    void stop() override {}

private:
    bcostars::RpcServicePrx m_proxy;
};

}  // namespace bcostars