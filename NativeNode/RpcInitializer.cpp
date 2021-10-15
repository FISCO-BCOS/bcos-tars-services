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
 * @brief initializer for the rpc
 * @file RPCInitializer.cpp
 * @author: octopus
 * @date 2021-07-15
 */
#include "RpcInitializer.h"
#include <bcos-boostssl/websocket/WsService.h>
#include <bcos-rpc/RpcFactory.h>
#include <include/BuildInfo.h>
#include <memory>

using namespace bcos;
using namespace bcos::initializer;

void RpcInitializer::init(bcos::tool::NodeConfig::Ptr _nodeConfig, const std::string& _configPath)
{
    bcos::rpc::NodeInfo nodeInfo;
    nodeInfo.nodeID = m_nodeID;
    nodeInfo.groupID = m_nodeConfig->groupId();
    nodeInfo.chainID = m_nodeConfig->chainId();
    nodeInfo.version = FISCO_BCOS_PROJECT_VERSION;
    nodeInfo.buildTime = FISCO_BCOS_BUILD_TIME;
    nodeInfo.gitCommitHash = FISCO_BCOS_COMMIT_HASH;
    nodeInfo.isSM = _nodeConfig->smCryptoType();
    nodeInfo.isWasm = _nodeConfig->isWasm();

    INITIALIZER_LOG(INFO) << LOG_BADGE("RpcInitializer::init") << LOG_KV("config", _configPath)
                          << LOG_KV("nodeID", nodeInfo.nodeID)
                          << LOG_KV("groupID", nodeInfo.groupID)
                          << LOG_KV("chainID", nodeInfo.chainID)
                          << LOG_KV("version", nodeInfo.version)
                          << LOG_KV("buildTime", nodeInfo.buildTime)
                          << LOG_KV("gitCommitHash", nodeInfo.gitCommitHash);

    auto factory = std::make_shared<bcos::rpc::RpcFactory>();
    factory->setLedger(m_ledger);
    factory->setTxPoolInterface(m_txPoolInterface);
    factory->setExecutorInterface(m_executorInterface);
    factory->setConsensusInterface(m_consensusInterface);
    factory->setBlockSyncInterface(m_blockSyncInterface);
    factory->setGatewayInterface(m_gatewayInterface);
    factory->setFrontServiceInterface(m_frontService);
    factory->setTransactionFactory(m_transactionFactory);
    factory->setKeyFactory(_nodeConfig->keyFactory());

    auto rpc = factory->buildRpc(_configPath, nodeInfo);
    auto amop = rpc->AMOP();
    auto amopWeak = std::weak_ptr(amop);
    auto rpcWeak = std::weak_ptr(rpc);
    auto wsService = rpc->wsService();
    auto wsServiceWeakPtr = std::weak_ptr<boostssl::ws::WsService>(wsService);

    // init AMOP message handler
    m_networkInitializer->registerMsgDispatcher(bcos::protocol::ModuleID::AMOP,
        [amopWeak](bcos::crypto::NodeIDPtr _nodeID, const std::string& _id, bytesConstRef _data) {
            auto amop = amopWeak.lock();
            if (amop)
            {
                amop->asyncNotifyAmopMessage(_nodeID, _id, _data, [](bcos::Error::Ptr _error) {
                    if (_error && _error->errorCode() != bcos::protocol::CommonError::SUCCESS)
                    {
                        INITIALIZER_LOG(WARNING)
                            << LOG_BADGE("RpcInitializer::init")
                            << LOG_DESC("asyncNotifyAmopMessage callback error")
                            << LOG_KV("errorCode", _error ? _error->errorCode() : -1)
                            << LOG_KV("errorMessage", _error ? _error->errorMessage() : "success");
                    }
                });
            }
        });

    m_networkInitializer->registerGetNodeIDsDispatcher(bcos::protocol::ModuleID::AMOP,
        [rpcWeak](std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
            bcos::front::ReceiveMsgFunc _receiveMsgCallback) {
            auto rpc = rpcWeak.lock();
            if (rpc)
            {
                rpc->asyncNotifyAmopNodeIDs(_nodeIDs, _receiveMsgCallback);
            }
        });

    m_rpcInterface = rpc;
}

void RpcInitializer::start()
{
    if (m_rpcInterface)
    {
        m_rpcInterface->start();
    }
}

void RpcInitializer::stop()
{
    if (m_rpcInterface)
    {
        m_rpcInterface->stop();
    }
}