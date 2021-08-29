#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/TarsUtils.h"
#include "../ExecutorService/ExecutorServiceClient.h"
#include "../GatewayService/GatewayServiceClient.h"
#include "../PBFTService/PBFTServiceClient.h"
#include "../StorageService/StorageServiceClient.h"
#include "../TxPoolService/TxPoolServiceClient.h"
#include "../libinitializer/ProtocolInitializer.h"
#include "Common.h"
#include "ExecutorService.h"
#include "PBFTService.h"
#include "RpcService.h"
#include "StorageService.h"
#include "servant/Servant.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-framework/interfaces/amop/AMOPInterface.h>
#include <bcos-framework/interfaces/rpc/RPCInterface.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-framework/libutilities/Log.h>
#include <bcos-ledger/libledger/Ledger.h>
#include <bcos-rpc/Rpc.h>
#include <bcos-rpc/RpcFactory.h>
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

    virtual void initialize() override;
    virtual void destroy() override;
    virtual void init();

    virtual bcostars::Error asyncNotifyBlockNumber(
        tars::Int64 blockNumber, tars::TarsCurrentPtr current) override;
    virtual bcostars::Error asyncNotifyAmopNodeIDs(
        const vector<vector<tars::Char> >& _nodeIDs, tars::TarsCurrentPtr current) override;
    virtual bcostars::Error asyncNotifyAmopMessage(const vector<tars::Char>& _nodeID,
        const std::string& _uuid, const vector<tars::Char>& _data,
        tars::TarsCurrentPtr current) override;

    bcos::ledger::Ledger::Ptr initLedger(
        bcos::initializer::ProtocolInitializer::Ptr protocolInitializer);
    bcos::rpc::RpcFactory::Ptr initRpcFactory(bcos::tool::NodeConfig::Ptr nodeConfig);

private:
    static std::once_flag m_initFlag;
    static bcos::rpc::Rpc::Ptr m_rpc;
    static bcos::crypto::KeyFactory::Ptr m_keyFactory;
    static std::atomic_bool m_running;
    static bcos::BoostLogInitializer::Ptr m_logInitializer;
};
}  // namespace bcostars