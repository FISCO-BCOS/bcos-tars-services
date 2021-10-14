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
 * @file GroupManagerServiceServer.h
 * @author: yujiechen
 * @date 2021-10-14
 */
#pragma once
#include <bcos-framework/interfaces/multigroup/ChainNodeInfoFactory.h>
#include <bcos-framework/interfaces/multigroup/GroupInfoFactory.h>
#include <bcos-framework/interfaces/multigroup/GroupManagerInterface.h>
#include <bcos-tars-protocol/tars/GroupManagerService.h>

namespace bcostars
{
struct GroupManagerServiceServerParam
{
    bcos::group::GroupManagerInterface::Ptr groupManager;
    bcos::group::GroupInfoFactory::Ptr groupInfoFactory;
    bcos::group::ChainNodeInfoFactory::Ptr chainNodeInfoFactory;
};
class GroupManagerServiceServer : public GroupManagerService
{
public:
    GroupManagerServiceServer(GroupManagerServiceServerParam const& _param)
      : m_groupManager(_param.groupManager),
        m_groupInfoFactory(_param.groupInfoFactory),
        m_chainNodeInfoFactory(_param.chainNodeInfoFactory)
    {}

    ~GroupManagerServiceServer() override {}

    void initialize() override {}
    void destroy() override {}

    Error asyncCreateGroup(const GroupInfo& _groupInfo, tars::TarsCurrentPtr current) override;
    Error asyncExpandGroupNode(const std::string& _chainID, const std::string& _groupID,
        const ChainNodeInfo& _nodeInfo, tars::TarsCurrentPtr current) override;
    Error asyncGetChainList(vector<std::string>& _chainList, tars::TarsCurrentPtr current) override;
    Error asyncGetGroupInfo(const std::string& _chainID, const std::string& _groupID,
        GroupInfo& _groupInfo, tars::TarsCurrentPtr current) override;
    Error asyncGetGroupInfos(const std::string& _chainID, const vector<std::string>& _groupList,
        vector<GroupInfo>& _groupInfo, tars::TarsCurrentPtr current) override;
    Error asyncGetGroupList(const std::string& _chainID, vector<std::string>& _groupList,
        tars::TarsCurrentPtr current) override;
    Error asyncGetNodeInfo(const std::string& _chainID, const std::string& _groupID,
        const std::string& _nodeName, ChainNodeInfo& _nodeInfo,
        tars::TarsCurrentPtr current) override;
    Error asyncRecoverGroup(const std::string& _chainID, const std::string& _groupID,
        tars::TarsCurrentPtr current) override;
    Error asyncRecoverGroupNode(const std::string& _chainID, const std::string& _groupID,
        const std::string& _nodeName, tars::TarsCurrentPtr current) override;
    Error asyncRemoveGroup(const std::string& _chainID, const std::string& _groupID,
        tars::TarsCurrentPtr current) override;
    Error asyncRemoveGroupNode(const std::string& _chainID, const std::string& _groupID,
        const std::string& _nodeName, tars::TarsCurrentPtr current) override;
    Error asyncStartNode(const std::string& _chainID, const std::string& _groupID,
        const std::string& _nodeName, tars::TarsCurrentPtr current) override;
    Error asyncStopNode(const std::string& _chainID, const std::string& _groupID,
        const std::string& _nodeName, tars::TarsCurrentPtr current) override;

private:
    bcos::group::GroupManagerInterface::Ptr m_groupManager;
    bcos::group::GroupInfoFactory::Ptr m_groupInfoFactory;
    bcos::group::ChainNodeInfoFactory::Ptr m_chainNodeInfoFactory;
};
}  // namespace bcostars