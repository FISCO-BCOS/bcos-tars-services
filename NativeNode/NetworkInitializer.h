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
 * @file NetworkInitializer.h
 * @author: yujiechen
 * @date 2021-06-10
 */

#pragma once
#include "Common.h"
#include <bcos-framework/interfaces/front/FrontServiceInterface.h>
#include <bcos-framework/interfaces/gateway/GatewayInterface.h>
#include <bcos-framework/interfaces/protocol/Protocol.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <memory>

namespace bcos
{
namespace gateway
{
class Gateway;
}

namespace front
{
class FrontService;
}

namespace initializer
{
class NetworkInitializer
{
public:
    using Ptr = std::shared_ptr<NetworkInitializer>;
    using SendResponse =
        std::function<void(std::string const&, int, bcos::crypto::NodeIDPtr, bytesConstRef)>;
    NetworkInitializer() = default;
    virtual ~NetworkInitializer() { stop(); }

    virtual void init(std::string const& _configFilePath, bcos::tool::NodeConfig::Ptr _nodeConfig,
        bcos::crypto::NodeIDPtr _nodeID);
    virtual void start();
    virtual void stop();

    bcos::front::FrontServiceInterface::Ptr frontService();
    std::shared_ptr<bcos::gateway::Gateway> gateway() { return m_gateWay; }

    virtual void registerMsgDispatcher(bcos::protocol::ModuleID _moduleID,
        std::function<void(
            bcos::crypto::NodeIDPtr _nodeID, std::string const& _id, bytesConstRef _data)>
            _dispatcher);

    virtual void registerGetNodeIDsDispatcher(bcos::protocol::ModuleID _moduleID,
        std::function<void(std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
            bcos::front::ReceiveMsgFunc _receiveMsgCallback)>
            _dispatcher);

protected:
    virtual void initGateWay(std::string const& _configFilePath);
    virtual void initFrontService(
        bcos::tool::NodeConfig::Ptr _nodeConfig, bcos::crypto::NodeIDPtr _nodeID);

private:
    std::shared_ptr<bcos::gateway::Gateway> m_gateWay;
    std::shared_ptr<bcos::front::FrontService> m_frontService;
};
}  // namespace initializer
}  // namespace bcos