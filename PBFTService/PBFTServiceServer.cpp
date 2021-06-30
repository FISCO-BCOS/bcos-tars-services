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

#include "PBFTServiceServer.h"
#include "../Common/ErrorConverter.h"
#include "../Common/ProxyDesc.h"
#include "../DispatcherService/DispatcherServiceClient.h"
#include "../FrontService/FrontServiceClient.h"
#include "../StorageService/StorageServiceClient.h"
#include "../protocols/BlockImpl.h"
#include "Common.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>

using namespace bcostars;
using namespace bcos::crypto;
using namespace bcos::dispatcher;
using namespace bcos::consensus;
using namespace bcos::txpool;
using namespace bcos::sealer;
using namespace bcos::sync;
using namespace bcos::ledger;

void PBFTServiceServer::initialize()
{
    m_keyFactory = std::make_shared<KeyFactoryImpl>();
    auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>(m_keyFactory);
    boost::property_tree::ptree genesisConfig;
    auto genesisConfigPath = ServerConfig::BasePath + "config.genesis";
    boost::property_tree::read_ini(genesisConfigPath, genesisConfig);

    boost::property_tree::ptree iniConfig;
    auto iniConfigPath = ServerConfig::BasePath + "config.ini";
    boost::property_tree::read_ini(iniConfigPath, iniConfig);
    nodeConfig->loadConfig(iniConfig, genesisConfig);
    m_protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
    m_protocolInitializer->init(nodeConfig, true);

    // create the frontService
    auto frontServiceProxy =
        Application::getCommunicator()->stringToProxy<bcostars::FrontServicePrx>(
            getProxyDesc("FrontServiceObj"));
    m_frontService =
        std::make_shared<bcostars::FrontServiceClient>(frontServiceProxy, m_keyFactory);

    // create the storage
    auto storageProxy = Application::getCommunicator()->stringToProxy<bcostars::StorageServicePrx>(
        getProxyDesc("StorageServiceObj"));
    m_storage = std::make_shared<bcostars::StorageServiceClient>(storageProxy);

    // create dispatcher
    auto dispatcherProxy =
        Application::getCommunicator()->stringToProxy<bcostars::DispatcherServicePrx>(
            getProxyDesc("DispatcherServiceObj"));
    m_dispatcher = std::make_shared<bcostars::DispatcherServiceClient>(dispatcherProxy);

    // create the ledger
    m_ledger =
        std::make_shared<bcos::ledger::Ledger>(m_protocolInitializer->blockFactory(), m_storage);

    // TODO: create the txpool client only
    createTxPool(nodeConfig);
    createSealer(nodeConfig);
    createPBFT(nodeConfig);
    createBlockSync(nodeConfig);

    // init and start all the modules
    m_txpool->init(m_sealer);
    m_sealer->init(m_pbft);
    m_blockSync->init();
    m_pbft->init(m_blockSync);

    m_txpool->start();
    m_sealer->start();
    m_pbft->start();
}

void PBFTServiceServer::createTxPool(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    auto txpoolFactory =
        std::make_shared<TxPoolFactory>(m_protocolInitializer->keyPair()->publicKey(),
            m_protocolInitializer->cryptoSuite(), m_protocolInitializer->txResultFactory(),
            m_protocolInitializer->blockFactory(), m_frontService, m_ledger, _nodeConfig->groupId(),
            _nodeConfig->chainId(), _nodeConfig->blockLimit());

    m_txpool = txpoolFactory->createTxPool();
    auto txpoolConfig = m_txpool->txpoolConfig();
    txpoolConfig->setPoolLimit(_nodeConfig->txpoolLimit());
    txpoolConfig->setNotifierWorkerNum(_nodeConfig->notifyWorkerNum());
    txpoolConfig->setVerifyWorkerNum(_nodeConfig->verifierWorkerNum());
}

void PBFTServiceServer::createSealer(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    // create sealer
    auto sealerFactory = std::make_shared<SealerFactory>(
        m_protocolInitializer->blockFactory(), m_txpool, _nodeConfig->minSealTime());
    m_sealer = sealerFactory->createSealer();
}

void PBFTServiceServer::createPBFT(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    auto keyPair = m_protocolInitializer->keyPair();
    // create pbft
    auto pbftFactory = std::make_shared<PBFTFactory>(m_protocolInitializer->cryptoSuite(),
        m_protocolInitializer->keyPair(), m_frontService, m_storage, m_ledger, m_txpool, m_sealer,
        m_dispatcher, m_protocolInitializer->blockFactory(),
        m_protocolInitializer->txResultFactory());
    m_pbft = pbftFactory->createPBFT();
    auto pbftConfig = m_pbft->pbftEngine()->pbftConfig();
    pbftConfig->setCheckPointTimeoutInterval(_nodeConfig->checkPointTimeoutInterval());
}

void PBFTServiceServer::createBlockSync(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    // create sync
    auto keyPair = m_protocolInitializer->keyPair();
    auto blockSyncFactory = std::make_shared<BlockSyncFactory>(keyPair->publicKey(),
        m_protocolInitializer->blockFactory(), m_protocolInitializer->txResultFactory(), m_ledger,
        m_txpool, m_frontService, m_dispatcher, m_pbft);
    m_blockSync = blockSyncFactory->createBlockSync();
}

Error PBFTServiceServer::asyncCheckBlock(
    const Block& _block, tars::Bool&, tars::TarsCurrentPtr _current)
{
    _current->setResponse(false);
    auto block = std::make_shared<bcostars::protocol::BlockImpl>(
        m_blockFactory->transactionFactory(), m_blockFactory->receiptFactory());
    block->setInner(_block);
    m_pbft->asyncCheckBlock(block, [_current](bcos::Error::Ptr _error, bool _verifyResult) {
        async_response_asyncCheckBlock(_current, toTarsError(_error), _verifyResult);
    });
}

Error PBFTServiceServer::asyncGetPBFTView(tars::Int64& _view, tars::TarsCurrentPtr _current)
{
    _current->setResponse(false);
    m_pbft->asyncGetPBFTView([_current](bcos::Error::Ptr _error, bcos::consensus::ViewType _view) {
        async_response_asyncGetPBFTView(_current, toTarsError(_error), _view);
    });
}


Error PBFTServiceServer::asyncNoteUnSealedTxsSize(
    tars::Int64 _unsealedTxsSize, tars::TarsCurrentPtr _current)
{
    _current->setResponse(false);
    m_sealer->asyncNoteUnSealedTxsSize(_unsealedTxsSize, [_current](bcos::Error::Ptr _error) {
        async_response_asyncNoteUnSealedTxsSize(_current, toTarsError(_error));
    });
}

Error PBFTServiceServer::asyncNotifyConsensusMessage(std::string const& _uuid,
    const vector<tars::UInt8>& _nodeId, const vector<tars::UInt8>& _data,
    tars::TarsCurrentPtr _current)
{
    _current->setResponse(false);
    auto nodeId = m_keyFactory->createKey(_nodeId);
    m_pbft->asyncNotifyConsensusMessage(
        nullptr, _uuid, nodeId, bcos::ref(_data), [_current](bcos::Error::Ptr _error) {
            async_response_asyncNotifyConsensusMessage(_current, toTarsError(_error));
        });
}


Error PBFTServiceServer::asyncNotifyNewBlock(
    const LedgerConfig& _ledgerConfig, tars::TarsCurrentPtr _current)
{
    _current->setResponse(false);
    auto ledgerConfig = toLedgerConfig(_ledgerConfig, m_keyFactory);
    m_pbft->asyncNotifyNewBlock(ledgerConfig, [_current](bcos::Error::Ptr _error) {
        async_response_asyncNotifyNewBlock(_current, toTarsError(_error));
    });
}

Error PBFTServiceServer::asyncSubmitProposal(const vector<tars::UInt8>& _proposalData,
    tars::Int64 _proposalIndex, const vector<tars::UInt8>& _proposalHash,
    tars::TarsCurrentPtr _current)
{
    _current->setResponse(false);
    auto proposalHash = bcos::crypto::HashType();
    if (_proposalHash.size() >= bcos::crypto::HashType::size)
    {
        proposalHash = bcos::crypto::HashType(_proposalHash.data(), bcos::crypto::HashType::size);
    }
    m_pbft->asyncSubmitProposal(bcos::ref(_proposalData), _proposalIndex, proposalHash,
        [_current](bcos::Error::Ptr _error) {
            async_response_asyncSubmitProposal(_current, toTarsError(_error));
        });
}