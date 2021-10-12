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
 * @file RpcInitializer.h
 * @author: octopus
 * @date 2021-07-15
 */
#pragma once
#include "libinitializer/Common.h"
#include "libinitializer/NetworkInitializer.h"
#include <bcos-framework/interfaces/consensus/ConsensusInterface.h>
#include <bcos-framework/interfaces/executor/ExecutorInterface.h>
#include <bcos-framework/interfaces/front/FrontServiceInterface.h>
#include <bcos-framework/interfaces/gateway/GatewayInterface.h>
#include <bcos-framework/interfaces/rpc/RPCInterface.h>
#include <bcos-framework/interfaces/sync/BlockSyncInterface.h>
#include <bcos-framework/interfaces/txpool/TxPoolInterface.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-ledger/libledger/Ledger.h>

namespace bcos
{
namespace initializer
{
class RpcInitializer
{
public:
    using Ptr = std::shared_ptr<RpcInitializer>;

    RpcInitializer() = default;
    ~RpcInitializer() { stop(); }

public:
    void init(bcos::tool::NodeConfig::Ptr, const std::string& _configPath);

    void start();
    void stop();

public:
    bcos::rpc::RPCInterface::Ptr rpcInterface() { return m_rpcInterface; }

    bcos::ledger::Ledger::Ptr ledger() const { return m_ledger; }
    void setLedger(bcos::ledger::Ledger::Ptr _ledger) { m_ledger = _ledger; }

    std::shared_ptr<bcos::executor::ExecutorInterface> executorInterface() const
    {
        return m_executorInterface;
    }
    void setExecutorInterface(std::shared_ptr<bcos::executor::ExecutorInterface> _executorInterface)
    {
        m_executorInterface = _executorInterface;
    }

    bcos::txpool::TxPoolInterface::Ptr txPoolInterface() const { return m_txPoolInterface; }
    void setTxPoolInterface(bcos::txpool::TxPoolInterface::Ptr _txPoolInterface)
    {
        m_txPoolInterface = _txPoolInterface;
    }

    bcos::consensus::ConsensusInterface::Ptr consensusInterface() const
    {
        return m_consensusInterface;
    }
    void setConsensusInterface(bcos::consensus::ConsensusInterface::Ptr _consensusInterface)
    {
        m_consensusInterface = _consensusInterface;
    }

    bcos::sync::BlockSyncInterface::Ptr blockSyncInterface() const { return m_blockSyncInterface; }
    void setBlockSyncInterface(bcos::sync::BlockSyncInterface::Ptr _blockSyncInterface)
    {
        m_blockSyncInterface = _blockSyncInterface;
    }

    bcos::gateway::GatewayInterface::Ptr gatewayInterface() const { return m_gatewayInterface; }
    void setGatewayInterface(bcos::gateway::GatewayInterface::Ptr _gatewayInterface)
    {
        m_gatewayInterface = _gatewayInterface;
    }

    bcos::front::FrontServiceInterface::Ptr frontService() const { return m_frontService; }
    void setFrontService(bcos::front::FrontServiceInterface::Ptr _frontService)
    {
        m_frontService = _frontService;
    }

    NetworkInitializer::Ptr networkInitializer() const { return m_networkInitializer; }
    void setNetworkInitializer(NetworkInitializer::Ptr _networkInitializer)
    {
        m_networkInitializer = _networkInitializer;
    }

    void setTransactionFactory(bcos::protocol::TransactionFactory::Ptr _transactionFactory)
    {
        m_transactionFactory = _transactionFactory;
    }

    void setNodeConfig(bcos::tool::NodeConfig::Ptr _nodeConfig) { m_nodeConfig = _nodeConfig; }
    bcos::tool::NodeConfig::Ptr nodeConfig() const { return m_nodeConfig; }

    void setNodeID(const std::string& _nodeID) { m_nodeID = _nodeID; }
    std::string nodeID() { return m_nodeID; }

public:
    std::string m_nodeID;
    NetworkInitializer::Ptr m_networkInitializer;
    bcos::front::FrontServiceInterface::Ptr m_frontService;
    bcos::rpc::RPCInterface::Ptr m_rpcInterface;
    bcos::ledger::Ledger::Ptr m_ledger;
    std::shared_ptr<bcos::executor::ExecutorInterface> m_executorInterface;
    bcos::txpool::TxPoolInterface::Ptr m_txPoolInterface;
    bcos::consensus::ConsensusInterface::Ptr m_consensusInterface;
    bcos::sync::BlockSyncInterface::Ptr m_blockSyncInterface;
    bcos::protocol::TransactionFactory::Ptr m_transactionFactory;
    bcos::gateway::GatewayInterface::Ptr m_gatewayInterface;
    bcos::tool::NodeConfig::Ptr m_nodeConfig;
};
}  // namespace initializer
}  // namespace bcos
