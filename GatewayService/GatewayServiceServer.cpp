#include "GatewayServiceServer.h"
#include <bcos-tars-protocol/Common.h>
using namespace bcostars;
bcostars::Error GatewayServiceServer::asyncNotifyGroupInfo(
    const bcostars::GroupInfo& groupInfo, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    auto bcosGroupInfo = toBcosGroupInfo(m_gatewayInitializer->chainNodeInfoFactory(),
        m_gatewayInitializer->groupInfoFactory(), groupInfo);
    m_gatewayInitializer->gateway()->asyncNotifyGroupInfo(
        bcosGroupInfo, [current](bcos::Error::Ptr&& _error) {
            async_response_asyncNotifyGroupInfo(current, toTarsError(_error));
        });
    return bcostars::Error();
}