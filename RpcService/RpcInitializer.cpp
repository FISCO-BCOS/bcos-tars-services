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
 * @brief initializer for the RpcService
 * @file RpcInitializer.cpp
 * @author: yujiechen
 * @date 2021-10-15
 */
#include "RpcInitializer.h"
#include "../libinitializer/ProtocolInitializer.h"
#include <bcos-tars-protocol/client/GatewayServiceClient.h>
using namespace bcos::group;
using namespace bcostars;

void RpcInitializer::init(std::string const& _configPath)
{
    // init node config
    RPCSERVICE_LOG(INFO) << LOG_DESC("init node config");
    auto nodeConfig =
        std::make_shared<bcos::tool::NodeConfig>(std::make_shared<bcos::crypto::KeyFactoryImpl>());
    nodeConfig->loadConfig(_configPath);
    RPCSERVICE_LOG(INFO) << LOG_DESC("init node config success")
                         << LOG_KV("configPath", _configPath);

    // init rpc config
    RPCSERVICE_LOG(INFO) << LOG_DESC("init rpc factory");
    auto factory = initRpcFactory(nodeConfig);
    RPCSERVICE_LOG(INFO) << LOG_DESC("init rpc factory success");

    auto config = factory->initConfig(_configPath);
    RPCSERVICE_LOG(INFO) << LOG_DESC("load rpc config success");

    auto rpc = factory->buildRpc(config);
    m_rpc = rpc;
}

bcos::rpc::RpcFactory::Ptr RpcInitializer::initRpcFactory(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    // init the protocol
    auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
    protocolInitializer->init(_nodeConfig);
    m_keyFactory = protocolInitializer->keyFactory();

    // get the gateway client
    auto gatewayPrx = Application::getCommunicator()->stringToProxy<GatewayServicePrx>(
        _nodeConfig->gatewayServiceName());
    auto gateway =
        std::make_shared<GatewayServiceClient>(gatewayPrx, protocolInitializer->keyFactory());

    auto factory = std::make_shared<bcos::rpc::RpcFactory>(
        _nodeConfig->chainId(), gateway, protocolInitializer->keyFactory());
    RPCSERVICE_LOG(INFO) << LOG_DESC("create rpc factory success");
    return factory;
}

void RpcInitializer::start()
{
    if (m_running)
    {
        RPCSERVICE_LOG(INFO) << LOG_DESC("The RpcService has already been started");
        return;
    }
    m_running = true;
    RPCSERVICE_LOG(INFO) << LOG_DESC("start rpc");
    m_rpc->start();
    RPCSERVICE_LOG(INFO) << LOG_DESC("start rpc success");
}

void RpcInitializer::stop()
{
    if (!m_running)
    {
        RPCSERVICE_LOG(WARNING) << LOG_DESC("The RpcService has already stopped!");
        return;
    }
    m_running = false;
    RPCSERVICE_LOG(INFO) << LOG_DESC("Stop the RpcService");

    if (m_rpc)
    {
        m_rpc->stop();
    }
    RPCSERVICE_LOG(INFO) << LOG_DESC("Stop the RpcService success");
}
