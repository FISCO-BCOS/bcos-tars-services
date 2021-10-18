#pragma once
#include "../Common/TarsUtils.h"
#include "RpcInitializer.h"
#include <bcos-tars-protocol/tars/RpcService.h>
namespace bcostars
{
struct RpcServiceParam
{
    RpcInitializer::Ptr rpcInitializer;
};
class RpcServiceServer : public bcostars::RpcService
{
public:
    RpcServiceServer(RpcServiceParam const& _param) : m_rpcInitializer(_param.rpcInitializer) {}
    virtual ~RpcServiceServer() {}

    void initialize() override {}
    void destroy() override {}

    bcostars::Error asyncNotifyBlockNumber(const std::string& _groupID,
        const std::string& _nodeName, tars::Int64 blockNumber,
        tars::TarsCurrentPtr current) override;
    bcostars::Error asyncNotifyAmopNodeIDs(
        const vector<vector<tars::Char> >& _nodeIDs, tars::TarsCurrentPtr current) override;
    bcostars::Error asyncNotifyAmopMessage(const vector<tars::Char>& _nodeID,
        const std::string& _uuid, const vector<tars::Char>& _data,
        tars::TarsCurrentPtr current) override;
    bcostars::Error asyncNotifyGroupInfo(
        const bcostars::GroupInfo& groupInfo, tars::TarsCurrentPtr current) override;
    bcos::rpc::RpcFactory::Ptr initRpcFactory(bcos::tool::NodeConfig::Ptr nodeConfig);

private:
    RpcInitializer::Ptr m_rpcInitializer;
};
}  // namespace bcostars