#pragma once

#include "../Common/TarsUtils.h"
#include "../libinitializer/ProtocolInitializer.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-framework/interfaces/rpc/RPCInterface.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-framework/libutilities/Log.h>
#include <bcos-rpc/Rpc.h>
#include <bcos-rpc/RpcFactory.h>
#include <bcos-tars-protocol/ErrorConverter.h>
#include <bcos-tars-protocol/client/GatewayServiceClient.h>
#include <bcos-tars-protocol/client/GroupManagerServiceClient.h>
#include <bcos-tars-protocol/tars/RpcService.h>
#include <tarscpp/servant/Servant.h>
#include <memory>
#include <utility>

#define RPCSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[RPCSERVICE][INITIALIZER]"
#define RPCSERVICE_BADGE "[RPCSERVICE]"
namespace bcostars
{
class RpcServiceServer : public bcostars::RpcService
{
public:
    virtual ~RpcServiceServer() {}

    void initialize() override;
    void destroy() override;
    virtual void init();

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
    static std::once_flag m_initFlag;
    static bcos::rpc::Rpc::Ptr m_rpc;
    static bcos::crypto::KeyFactory::Ptr m_keyFactory;
    static std::atomic_bool m_running;
    static bcos::BoostLogInitializer::Ptr m_logInitializer;

    static bcos::group::GroupInfoFactory::Ptr m_groupInfoFactory;
    static bcos::group::ChainNodeInfoFactory::Ptr m_chainNodeInfoFactory;
};
}  // namespace bcostars