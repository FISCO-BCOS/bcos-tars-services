#include "RpcServiceServer.h"
#include <bcos-tars-protocol/Common.h>
#include <bcos-tars-protocol/ErrorConverter.h>
#include <tarscpp/servant/Servant.h>
#include <memory>

using namespace bcostars;
bcostars::Error RpcServiceServer::asyncNotifyBlockNumber(const std::string& _groupID,
    const std::string& _nodeName, tars::Int64 blockNumber, tars::TarsCurrentPtr current)
{
    current->setResponse(false);

    m_rpcInitializer->rpc()->asyncNotifyBlockNumber(
        _groupID, _nodeName, blockNumber, [current, blockNumber](bcos::Error::Ptr _error) {
            RPCSERVICE_LOG(DEBUG) << LOG_BADGE("asyncNotifyBlockNumber")
                                  << LOG_KV("blockNumber", blockNumber)
                                  << LOG_KV("errorCode", _error ? _error->errorCode() : 0)
                                  << LOG_KV("errorMessage", _error ? _error->errorMessage() : "");
            async_response_asyncNotifyBlockNumber(current, toTarsError(_error));
        });

    return bcostars::Error();
}

bcostars::Error RpcServiceServer::asyncNotifyGroupInfo(
    const bcostars::GroupInfo& groupInfo, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    auto bcosGroupInfo = toBcosGroupInfo(
        m_rpcInitializer->chainNodeInfoFactory(), m_rpcInitializer->groupInfoFactory(), groupInfo);
    m_rpcInitializer->rpc()->asyncNotifyGroupInfo(
        bcosGroupInfo, [current](bcos::Error::Ptr&& _error) {
            async_response_asyncNotifyGroupInfo(current, toTarsError(_error));
        });
    return bcostars::Error();
}