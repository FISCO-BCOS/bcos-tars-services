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
 * @brief Application for the NativeNode
 * @file NativeNodeApp.h
 * @author: yujiechen
 * @date 2021-10-18
 */
#pragma once
#include "../Common/TarsUtils.h"
#include "../LedgerService/LedgerServiceServer.h"
#include "../PBFTService/PBFTServiceServer.h"
#include "../SchedulerService/SchedulerServiceServer.h"
#include "../TxPoolService/TxPoolServiceServer.h"
#include "Initializer.h"
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <tarscpp/servant/Application.h>

namespace bcostars
{
class NativeNodeApp : public Application
{
public:
    NativeNodeApp()
      : m_iniConfigPath(ServerConfig::BasePath + "config.ini"),
        m_genesisConfigPath(ServerConfig::BasePath + "config.genesis")
    {
        addAppConfig("node.pem");
        addAppConfig("config.genesis");
        addAppConfig("config.ini");
    }
    ~NativeNodeApp() override {}

    void initialize() override;
    void destroyApp() override { m_nodeInitializer->stop(); }

protected:
    virtual void initLog();
    virtual void initNodeService();
    virtual void initTarsNodeService();

private:
    std::string m_iniConfigPath;
    std::string m_genesisConfigPath;

    bcos::BoostLogInitializer::Ptr m_logInitializer;
    bcos::initializer::Initializer::Ptr m_nodeInitializer;

    TxPoolServiceServer::Ptr m_txpoolServer;
    PBFTServiceServer::Ptr m_pbftServer;
};
}  // namespace bcostars