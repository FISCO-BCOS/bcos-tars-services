/**
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @brief tars server for GroupManagerService
 * @file GroupManagerServiceServer.cpp
 * @author: yujiechen
 * @date 2021-10-14
 */
#include "GroupManagerServiceServer.h"
#include <bcos-tars-protocol/Common.h>
#include <bcos-tars-protocol/ErrorConverter.h>
using namespace bcostars;

Error GroupManagerServiceServer::asyncCreateGroup(
    const GroupInfo& _groupInfo, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    auto bcosGroupInfo = toBcosGroupInfo(m_chainNodeInfoFactory, m_groupInfoFactory, _groupInfo);
    m_groupManager->asyncCreateGroup(bcosGroupInfo, [current](bcos::Error::Ptr&& _error) {
        async_response_asyncCreateGroup(current, toTarsError(_error));
    });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncExpandGroupNode(const std::string& _chainID,
    const std::string& _groupID, const ChainNodeInfo& _nodeInfo, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    auto ChainNodeInfo = toBcosChainNodeInfo(m_chainNodeInfoFactory, _nodeInfo);
    m_groupManager->asyncExpandGroupNode(
        _chainID, _groupID, ChainNodeInfo, [current](bcos::Error::Ptr&& _error) {
            async_response_asyncExpandGroupNode(current, toTarsError(_error));
        });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncGetChainList(
    vector<std::string>& _chainList, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncGetChainList(
        [current](bcos::Error::Ptr&& _error, std::set<std::string>&& _chainList) {
            std::vector<std::string> chainList(_chainList.begin(), _chainList.end());
            async_response_asyncGetChainList(current, toTarsError(_error), chainList);
        });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncGetGroupInfo(const std::string& _chainID,
    const std::string& _groupID, GroupInfo&, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncGetGroupInfo(_chainID, _groupID,
        [current](bcos::Error::Ptr&& _error, bcos::group::GroupInfo::Ptr&& _groupInfo) {
            async_response_asyncGetGroupInfo(
                current, toTarsError(_error), toTarsGroupInfo(_groupInfo));
        });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncGetGroupInfos(const std::string& _chainID,
    const vector<std::string>& _groupList, vector<GroupInfo>&, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncGetGroupInfos(_chainID, _groupList,
        [current](
            bcos::Error::Ptr&& _error, std::vector<bcos::group::GroupInfo::Ptr>&& _groupInfos) {
            vector<GroupInfo> tarsGroupInfoList;
            for (auto const& groupInfo : _groupInfos)
            {
                tarsGroupInfoList.emplace_back(toTarsGroupInfo(groupInfo));
            }
            async_response_asyncGetGroupInfos(current, toTarsError(_error), tarsGroupInfoList);
        });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncGetGroupList(
    const std::string& _chainID, vector<std::string>&, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncGetGroupList(
        _chainID, [current](bcos::Error::Ptr&& _error, std::set<std::string>&& _groupList) {
            std::vector<std::string> groupList(_groupList.begin(), _groupList.end());
            async_response_asyncGetGroupList(current, toTarsError(_error), groupList);
        });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncGetNodeInfo(const std::string& _chainID,
    const std::string& _groupID, const std::string& _nodeName, ChainNodeInfo& _nodeInfo,
    tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncGetNodeInfo(_chainID, _groupID, _nodeName,
        [current](bcos::Error::Ptr&& _error, bcos::group::ChainNodeInfo::Ptr&& _chainNodeInfo) {
            auto tarsNodeInfo = toTarsChainNodeInfo(_chainNodeInfo);
            async_response_asyncGetNodeInfo(current, toTarsError(_error), tarsNodeInfo);
        });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncRecoverGroup(
    const std::string& _chainID, const std::string& _groupID, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncRecoverGroup(_chainID, _groupID, [current](bcos::Error::Ptr&& _error) {
        async_response_asyncRecoverGroup(current, toTarsError(_error));
    });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncRecoverGroupNode(const std::string& _chainID,
    const std::string& _groupID, const std::string& _nodeName, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncRecoverGroupNode(
        _chainID, _groupID, _nodeName, [current](bcos::Error::Ptr&& _error) {
            async_response_asyncRecoverGroupNode(current, toTarsError(_error));
        });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncRemoveGroup(
    const std::string& _chainID, const std::string& _groupID, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncRemoveGroup(_chainID, _groupID, [current](bcos::Error::Ptr&& _error) {
        async_response_asyncRemoveGroup(current, toTarsError(_error));
    });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncRemoveGroupNode(const std::string& _chainID,
    const std::string& _groupID, const std::string& _nodeName, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncRemoveGroupNode(
        _chainID, _groupID, _nodeName, [current](bcos::Error::Ptr&& _error) {
            async_response_asyncRemoveGroupNode(current, toTarsError(_error));
        });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncStartNode(const std::string& _chainID,
    const std::string& _groupID, const std::string& _nodeName, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncStartNode(
        _chainID, _groupID, _nodeName, [current](bcos::Error::Ptr&& _error) {
            async_response_asyncStartNode(current, toTarsError(_error));
        });
    return bcostars::Error();
}

Error GroupManagerServiceServer::asyncStopNode(const std::string& _chainID,
    const std::string& _groupID, const std::string& _nodeName, tars::TarsCurrentPtr current)
{
    current->setResponse(false);
    m_groupManager->asyncStopNode(
        _chainID, _groupID, _nodeName, [current](bcos::Error::Ptr _error) {
            async_response_asyncStopNode(current, toTarsError(_error));
        });
    return bcostars::Error();
}