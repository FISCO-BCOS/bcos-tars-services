#include "GatewayServiceServer.h"
#include <bcos-tars-protocol/Common.h>
using namespace bcostars;

std::once_flag GatewayServiceServer::m_initFlag;
bcos::gateway::Gateway::Ptr GatewayServiceServer::m_gateway;
bcos::BoostLogInitializer::Ptr GatewayServiceServer::m_logInitializer;
bcos::crypto::KeyFactory::Ptr GatewayServiceServer::m_keyFactory;

bcos::group::GroupInfoFactory::Ptr GatewayServiceServer::m_groupInfoFactory;
bcos::group::ChainNodeInfoFactory::Ptr GatewayServiceServer::m_chainNodeInfoFactory;

bcostars::Error GatewayServiceServer::asyncNotifyGroupInfo(
    const bcostars::GroupInfo& groupInfo, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    auto bcosGroupInfo = toBcosGroupInfo(m_chainNodeInfoFactory, m_groupInfoFactory, groupInfo);
    m_gateway->asyncNotifyGroupInfo(bcosGroupInfo, [current](bcos::Error::Ptr&& _error) {
        async_response_asyncNotifyGroupInfo(current, toTarsError(_error));
    });
    return bcostars::Error();
}