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
 * @brief initializer for the PBFT module
 * @file PBFTInitializer.h
 * @author: yujiechen
 * @date 2021-06-10
 */
#pragma once
#include "NetworkInitializer.h"
#include "ProtocolInitializer.h"
#include <bcos-framework/libsealer/SealerFactory.h>
#include <bcos-pbft/pbft/PBFTFactory.h>
#include <bcos-sync/BlockSyncFactory.h>
#include <bcos-txpool/TxPoolFactory.h>

namespace bcos
{
namespace initializer
{
class PBFTInitializer
{
public:
    using Ptr = std::shared_ptr<PBFTInitializer>;
    PBFTInitializer() = default;
    virtual ~PBFTInitializer() { stop(); }

    virtual void init(bcos::tool::NodeConfig::Ptr _nodeConfig,
        ProtocolInitializer::Ptr _protocolInitializer, NetworkInitializer::Ptr _networkInitializer,
        bcos::ledger::LedgerInterface::Ptr _ledger,
        bcos::scheduler::SchedulerInterface::Ptr scheduler,
        bcos::storage::StorageInterface::Ptr _storage);

    virtual void start();
    virtual void stop();

    // for mini-consensus
    bcos::txpool::TxPoolInterface::Ptr txpool() { return m_txpool; }
    bcos::sync::BlockSync::Ptr blockSync() { return m_blockSync; }
    bcos::consensus::PBFTImpl::Ptr pbft() { return m_pbft; }


protected:
    virtual void createTxPool(bcos::tool::NodeConfig::Ptr _nodeConfig,
        ProtocolInitializer::Ptr _protocolInitializer, NetworkInitializer::Ptr _networkInitializer,
        bcos::ledger::LedgerInterface::Ptr _ledger);

    virtual void createSealer(
        bcos::tool::NodeConfig::Ptr _nodeConfig, ProtocolInitializer::Ptr _protocolInitializer);

    virtual void createPBFT(bcos::tool::NodeConfig::Ptr _nodeConfig,
        ProtocolInitializer::Ptr _protocolInitializer, NetworkInitializer::Ptr _networkInitializer,
        bcos::storage::StorageInterface::Ptr _storage, bcos::ledger::LedgerInterface::Ptr _ledger,
        bcos::scheduler::SchedulerInterface::Ptr _dispatcher);

    virtual void createSync(bcos::tool::NodeConfig::Ptr _nodeConfig,
        ProtocolInitializer::Ptr _protocolInitializer, NetworkInitializer::Ptr _networkInitializer,
        bcos::ledger::LedgerInterface::Ptr _ledger,
        bcos::scheduler::SchedulerInterface::Ptr _dispatcher);

    virtual void registerHandlers();

private:
    bcos::sealer::Sealer::Ptr m_sealer;
    bcos::sync::BlockSync::Ptr m_blockSync;
    bcos::txpool::TxPool::Ptr m_txpool;
    bcos::consensus::PBFTImpl::Ptr m_pbft;
};
}  // namespace initializer
}  // namespace bcos