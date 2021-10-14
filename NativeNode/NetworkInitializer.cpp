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
 * @brief initializer for the network
 * @file NetworkInitializer.cpp
 * @author: yujiechen
 * @date 2021-06-10
 */
#include "NetworkInitializer.h"
#include <bcos-front/FrontServiceFactory.h>
#include <bcos-tars-protocol/client/GatewayServiceClient.h>
#include <tarscpp/servant/Application.h>
#include <tarscpp/servant/Servant.h>

using namespace bcos;
using namespace bcos::tool;
using namespace bcos::initializer;
using namespace bcos::gateway;
using namespace bcos::front;
using namespace bcos::crypto;
using namespace bcos::protocol;

void NetworkInitializer::init(NodeConfig::Ptr _nodeConfig, NodeIDPtr _nodeID)
{
    INITIALIZER_LOG(INFO) << LOG_DESC("init the network") << LOG_KV("nodeId", _nodeID->shortHex())
                          << LOG_KV("groupId", _nodeConfig->groupId());
    initGateWay(_nodeConfig);
    initFrontService(_nodeConfig, _nodeID);
}

FrontServiceInterface::Ptr NetworkInitializer::frontService()
{
    return m_frontService;
}

void NetworkInitializer::initGateWay(NodeConfig::Ptr _nodeConfig)
{
    // get the gateway client
    auto gatewayPrx = Application::getCommunicator()->stringToProxy<bcostars::GatewayServicePrx>(
        _nodeConfig->gatewayServiceName());
    m_gateWay = std::make_shared<bcostars::GatewayServiceClient>(
        gatewayPrx, m_protocol->cryptoSuite()->keyFactory());
}

void NetworkInitializer::initFrontService(NodeConfig::Ptr _nodeConfig, NodeIDPtr _nodeID)
{
    auto frontServiceFactory = std::make_shared<FrontServiceFactory>();
    frontServiceFactory->setGatewayInterface(m_gateWay);
    auto threadPool = std::make_shared<ThreadPool>("frontService", 1);
    frontServiceFactory->setThreadPool(threadPool);
    m_frontService = frontServiceFactory->buildFrontService(_nodeConfig->groupId(), _nodeID);
}

void NetworkInitializer::start()
{
    m_frontService->start();
}
void NetworkInitializer::stop()
{
    m_frontService->stop();
}

void NetworkInitializer::registerMsgDispatcher(ModuleID _moduleID,
    std::function<void(NodeIDPtr _nodeID, std::string const& _id, bytesConstRef _data)> _dispatcher)
{
    m_frontService->registerModuleMessageDispatcher(_moduleID, _dispatcher);
}

void NetworkInitializer::registerGetNodeIDsDispatcher(ModuleID _moduleID,
    std::function<void(std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
        bcos::front::ReceiveMsgFunc _receiveMsgCallback)>
        _dispatcher)
{
    m_frontService->registerModuleNodeIDsDispatcher(_moduleID, _dispatcher);
}