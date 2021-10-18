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

bcostars::Error RpcServiceServer::asyncNotifyAmopNodeIDs(
    const vector<vector<tars::Char> >& _nodeIDs, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    std::shared_ptr<bcos::crypto::NodeIDs> nodeIDs = std::make_shared<bcos::crypto::NodeIDs>();
    for (const auto& nodeID : _nodeIDs)
    {
        auto nodeIDPtr = m_rpcInitializer->keyFactory()->createKey(
            bcos::bytesConstRef((const bcos::byte*)nodeID.data(), nodeID.size()));
        nodeIDs->push_back(nodeIDPtr);
    }

    m_rpcInitializer->rpc()->asyncNotifyAmopNodeIDs(
        nodeIDs, [current, nodeIDs](bcos::Error::Ptr _error) {
            RPCSERVICE_LOG(DEBUG) << LOG_BADGE("asyncNotifyAmopNodeIDs")
                                  << LOG_KV("nodeIDs size", nodeIDs->size())
                                  << LOG_KV("errorCode", _error ? _error->errorCode() : 0)
                                  << LOG_KV("errorMessage", _error ? _error->errorMessage() : "");
            async_response_asyncNotifyAmopNodeIDs(current, toTarsError(_error));
        });

    return bcostars::Error();
}

bcostars::Error RpcServiceServer::asyncNotifyAmopMessage(const vector<tars::Char>& _nodeID,
    const std::string& _uuid, const vector<tars::Char>& _data, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    auto nodeIDPtr = m_rpcInitializer->keyFactory()->createKey(
        bcos::bytesConstRef((const bcos::byte*)_nodeID.data(), _nodeID.size()));
    m_rpcInitializer->rpc()->asyncNotifyAmopMessage(nodeIDPtr, _uuid,
        bcos::bytesConstRef((bcos::byte*)_data.data(), _data.size()),
        [current, nodeIDPtr](bcos::Error::Ptr _error) {
            RPCSERVICE_LOG(TRACE) << LOG_BADGE("asyncNotifyAmopMessage")
                                  << LOG_KV("nodeID", nodeIDPtr->hex())
                                  << LOG_KV("errorCode", _error ? _error->errorCode() : 0)
                                  << LOG_KV("errorMessage", _error ? _error->errorMessage() : "");
            async_response_asyncNotifyAmopMessage(current, toTarsError(_error));
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
