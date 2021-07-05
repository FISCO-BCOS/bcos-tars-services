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
 * @brief Common for the PBFTService
 * @file Common.h
 * @author: yujiechen
 * @date 2021-06-29
 */
#pragma once
#include <bcos-framework/interfaces/consensus/ConsensusNode.h>
#include <bcos-framework/interfaces/ledger/LedgerConfig.h>
namespace bcostars
{
inline bcos::ledger::LedgerConfig::Ptr toLedgerConfig(
    bcostars::LedgerConfig const& _ledgerConfig, bcos::crypto::KeyFactory::Ptr _keyFactory)
{
    auto ledgerConfig = std::make_shared<bcos::ledger::LedgerConfig>();
    bcos::consensus::ConsensusNodeList consensusNodeList;
    for (auto const& node : _ledgerConfig.consensusNodeList)
    {
        auto nodeID = _keyFactory->createKey(node.nodeID);
        consensusNodeList.push_back(
            std::make_shared<bcos::consensus::ConsensusNode>(nodeID, node.weight));
    }
    ledgerConfig->setConsensusNodeList(consensusNodeList);

    bcos::consensus::ConsensusNodeList observerNodeList;
    for (auto const& node : _ledgerConfig.observerNodeList)
    {
        auto nodeID = _keyFactory->createKey(node.nodeID);
        observerNodeList.push_back(
            std::make_shared<bcos::consensus::ConsensusNode>(nodeID, node.weight));
    }
    ledgerConfig->setObserverNodeList(observerNodeList);
    auto hash = bcos::crypto::HashType();
    if (_ledgerConfig.hash.size() >= bcos::crypto::HashType::size)
    {
        hash = bcos::crypto::HashType(_ledgerConfig.hash.data(), bcos::crypto::HashType::size);
    }
    ledgerConfig->setHash(hash);
    ledgerConfig->setBlockNumber(_ledgerConfig.blockNumber);
    ledgerConfig->setConsensusTimeout(_ledgerConfig.consensusTimeout);
    ledgerConfig->setBlockTxCountLimit(_ledgerConfig.blockTxCountLimit);
    ledgerConfig->setLeaderSwitchPeriod(_ledgerConfig.leaderSwitchPeriod);
    ledgerConfig->setSealerId(_ledgerConfig.sealerId);
    return ledgerConfig;
}

inline bcostars::LedgerConfig toTarsLedgerConfig(bcos::ledger::LedgerConfig::Ptr _ledgerConfig)
{
    bcostars::LedgerConfig ledgerConfig;
    ledgerConfig.hash = _ledgerConfig->hash().asBytes();
    ledgerConfig.blockNumber = _ledgerConfig->blockNumber();
    ledgerConfig.consensusTimeout = _ledgerConfig->consensusTimeout();
    ledgerConfig.blockTxCountLimit = _ledgerConfig->blockTxCountLimit();
    ledgerConfig.leaderSwitchPeriod = _ledgerConfig->leaderSwitchPeriod();
    ledgerConfig.sealerId = _ledgerConfig->sealerId();

    // set consensusNodeList
    auto const& consensusNodeList = _ledgerConfig->consensusNodeList();
    for (auto node : consensusNodeList)
    {
        bcostars::ConsensusNode consensusNode;
        consensusNode.nodeID = node->nodeID()->data();
        consensusNode.weight = node->weight();
        ledgerConfig.consensusNodeList.push_back(consensusNode);
    }
    // set observerNodeList
    auto const& observerNodeList = _ledgerConfig->observerNodeList();
    for (auto node : observerNodeList)
    {
        bcostars::ConsensusNode observerNode;
        observerNode.nodeID = node->nodeID()->data();
        observerNode.weight = node->weight();
        ledgerConfig.observerNodeList.push_back(observerNode);
    }
    return ledgerConfig;
}
}  // namespace bcostars