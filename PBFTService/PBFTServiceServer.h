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
 * @brief server for the PBFTService
 * @file PBFTService.h
 * @author: yujiechen
 * @date 2021-06-29
 */

#pragma once


#include "../libinitializer/ProtocolInitializer.h"
#include "PBFTService.h"
#include "servant/Application.h"
#include "servant/Communicator.h"
#include <bcos-framework/interfaces/crypto/KeyFactory.h>
#include <bcos-framework/libsealer/SealerFactory.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-ledger/ledger/Ledger.h>
#include <bcos-pbft/pbft/PBFTFactory.h>
#include <bcos-sync/BlockSyncFactory.h>
#include <bcos-txpool/TxPoolFactory.h>
#include <mutex>

#define PBFTSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[PBFTSERVICE]"

namespace bcostars
{
class PBFTServiceServer : public bcostars::PBFTService
{
public:
    using Ptr = std::shared_ptr<PBFTServiceServer>;
    PBFTServiceServer() {}
    ~PBFTServiceServer() override {}

    void initialize() override;
    void destroy() override {}

    bcostars::Error asyncCheckBlock(
        const bcostars::Block& _block, tars::Bool&, tars::TarsCurrentPtr _current) override;
    bcostars::Error asyncGetPBFTView(tars::Int64& _view, tars::TarsCurrentPtr _current) override;

    // Note: since the sealer is intergrated with the PBFT, this interfaces is useless now
    bcostars::Error asyncNoteUnSealedTxsSize(
        tars::Int64 _unsealedTxsSize, tars::TarsCurrentPtr _current) override;

    bcostars::Error asyncNotifyConsensusMessage(std::string const& _uuid,
        const vector<tars::UInt8>& _nodeId, const vector<tars::UInt8>& _data,
        tars::TarsCurrentPtr _current) override;

    bcostars::Error asyncNotifyBlockSyncMessage(std::string const& _uuid,
        const vector<tars::UInt8>& _nodeId, const vector<tars::UInt8>& _data,
        tars::TarsCurrentPtr _current) override;

    // Note: since the blockSync module is intergrated with the PBFT, this interfaces is useless now
    bcostars::Error asyncNotifyNewBlock(
        const bcostars::LedgerConfig& _ledgerConfig, tars::TarsCurrentPtr _current) override;

    // Note: since the sealer module is intergrated with the PBFT, the interface is useless now
    bcostars::Error asyncSubmitProposal(const vector<tars::UInt8>& _proposalData,
        tars::Int64 _proposalIndex, const vector<tars::UInt8>& _proposalHash,
        tars::TarsCurrentPtr _current) override;

protected:
    virtual void registerHandlers();

    // TODO: create the txpool client only
    virtual void createTxPool(bcos::tool::NodeConfig::Ptr _nodeConfig);
    virtual void createSealer(bcos::tool::NodeConfig::Ptr _nodeConfig);
    virtual void createBlockSync(bcos::tool::NodeConfig::Ptr _nodeConfig);
    virtual void createPBFT(bcos::tool::NodeConfig::Ptr _nodeConfig);

private:
    // the local dependencies
    bcos::txpool::TxPool::Ptr m_txpool;
    bcos::consensus::PBFTImpl::Ptr m_pbft;
    bcos::sealer::Sealer::Ptr m_sealer;
    bcos::sync::BlockSync::Ptr m_blockSync;

    // the client dependencies used to access the remote server service
    bcos::front::FrontServiceInterface::Ptr m_frontService;
    bcos::storage::StorageInterface::Ptr m_storage;
    bcos::ledger::LedgerInterface::Ptr m_ledger;
    bcos::dispatcher::DispatcherInterface::Ptr m_dispatcher;

    bcos::initializer::ProtocolInitializer::Ptr m_protocolInitializer;

    bcos::protocol::BlockFactory::Ptr m_blockFactory;
    bcos::crypto::KeyFactory::Ptr m_keyFactory;
};

}  // namespace bcostars