#include "RpcServiceServer.h"
#include <bcos-tars-protocol/Common.h>
#include <bcos-tars-protocol/ErrorConverter.h>
#include <bcos-tars-protocol/protocol/TransactionSubmitResultImpl.h>
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

bcostars::Error RpcServiceServer::asyncNotifyAMOPMessage(tars::Int32 _type,
    const std::string& _topic, const vector<tars::Char>& _requestData, vector<tars::Char>&,
    tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_rpcInitializer->rpc()->asyncNotifyAMOPMessage(_type, _topic,
        bcos::bytesConstRef((const bcos::byte*)_requestData.data(), _requestData.size()),
        [current](bcos::Error::Ptr&& _error, bcos::bytesPointer _responseData) {
            vector<tars::Char> response;
            if (_responseData)
            {
                response.assign(_responseData->begin(), _responseData->end());
            }
            async_response_asyncNotifyAMOPMessage(current, toTarsError(_error), response);
        });
    return bcostars::Error();
}

bcostars::Error RpcServiceServer::asyncNotifyTransactionResult(const std::string& _rpcID,
    const std::string& _groupID, const vector<tars::Char>& _txHash,
    const bcostars::TransactionSubmitResult& _result, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    bcos::crypto::HashType hash = bcos::crypto::HashType();
    if (_txHash.size() >= bcos::crypto::HashType::size)
    {
        hash = bcos::crypto::HashType(
            reinterpret_cast<const bcos::byte*>(_txHash.data()), bcos::crypto::HashType::size);
    }
    // TODO: remove this
    auto bcosResult = std::make_shared<bcostars::protocol::TransactionSubmitResultImpl>(nullptr,
        [inner = std::move(const_cast<bcostars::TransactionSubmitResult&>(_result))]() mutable {
            return &inner;
        });
    m_rpcInitializer->rpc()->asyncNotifyTransactionResult(_rpcID, _groupID, hash, bcosResult);
    async_response_asyncNotifyTransactionResult(current, toTarsError(nullptr));
    return bcostars::Error();
}