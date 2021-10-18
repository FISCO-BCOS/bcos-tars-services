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
 * @brief application for GroupManagerService
 * @file GroupManagerServiceApp.h
 * @author: yujiechen
 * @date 2021-10-18
 */
#pragma once
#include "../Common/TarsUtils.h"
#include "../libinitializer/Common.h"
#include "GroupManagerServiceServer.h"
#include <bcos-framework/interfaces/protocol/ServiceDesc.h>
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-group-manager/GroupManagerFactory.h>
#include <tarscpp/servant/Application.h>
namespace bcostars
{
class GroupManagerServiceApp : public tars::Application
{
public:
    GroupManagerServiceApp() : Application() {}

    virtual ~GroupManagerServiceApp() override{};

    virtual void initialize() override
    {
        m_iniConfigPath = ServerConfig::BasePath + "/config.ini";
        addAppConfig("config.ini");
        initService();
        GroupManagerServiceServerParam param;
        param.groupManager = m_groupManager;
        param.groupInfoFactory = m_groupInfoFactory;
        param.chainNodeInfoFactory = m_chainNodeInfoFactory;
        addServantWithParams<GroupManagerServiceServer, GroupManagerServiceServerParam>(
            ServerConfig::Application + "." + ServerConfig::ServerName + "." +
                bcos::protocol::GROUPMANAGER_SERVANT_NAME,
            param);
    }
    virtual void destroyApp() override {}

protected:
    virtual void initService();
    virtual bcos::storage::StorageInterface::Ptr createStorage(std::string const& _storagePath);

private:
    bcos::BoostLogInitializer::Ptr m_logInitializer;
    bcos::group::GroupManagerInterface::Ptr m_groupManager;
    bcos::group::GroupInfoFactory::Ptr m_groupInfoFactory;
    bcos::group::ChainNodeInfoFactory::Ptr m_chainNodeInfoFactory;
    std::string m_iniConfigPath;
};
}  // namespace bcostars