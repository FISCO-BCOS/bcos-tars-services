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
#include "../Common/TarsUtils.h"
#include "../DispatcherService/DispatcherServiceClient.h"
#include "../FrontService/FrontServiceClient.h"
#include "../StorageService/StorageServiceClient.h"
#include "../TxPoolService/TxPoolServiceClient.h"
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
    auto iniConfigPath = ServerConfig::BasePath + "config.ini";
    nodeConfig->loadConfig(iniConfigPath);

    // auto genesisConfigPath = ServerConfig::BasePath + "config.genesis";
    // nodeConfig->loadGenesisConfig(genesisConfigPath);

    m_protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
    m_protocolInitializer->init(nodeConfig);
    auto privateKeyPath = ServerConfig::BasePath + "node.pem";
    m_protocolInitializer->loadKeyPair(privateKeyPath);

    // create the frontService
    auto frontServiceProxy =
        Application::getCommunicator()->stringToProxy<bcostars::FrontServicePrx>(
            getProxyDesc(FRONT_SERVICE_NAME));
    m_frontService =
        std::make_shared<bcostars::FrontServiceClient>(frontServiceProxy, m_keyFactory);

    // create the storage
    auto storageProxy = Application::getCommunicator()->stringToProxy<bcostars::StorageServicePrx>(
        getProxyDesc(STORAGE_SERVICE_NAME));
    m_storage = std::make_shared<bcostars::StorageServiceClient>(storageProxy);

    // create dispatcher
    auto dispatcherProxy =
        Application::getCommunicator()->stringToProxy<bcostars::DispatcherServicePrx>(
            getProxyDesc(DISPATCHER_SERVANT_NAME));
    m_dispatcher = std::make_shared<bcostars::DispatcherServiceClient>(dispatcherProxy);

    // create the ledger
    auto ledger =
        std::make_shared<bcos::ledger::Ledger>(m_protocolInitializer->blockFactory(), m_storage);
    m_ledger = ledger;
    // write the genesis block through ledger
    // ledger->buildGenesisBlock(
    //    nodeConfig->ledgerConfig(), nodeConfig->txGasLimit(), nodeConfig->genesisData());

    // create the txpool client only
    createTxPool(nodeConfig);

    createSealer(nodeConfig);
    createPBFT(nodeConfig);
    createBlockSync(nodeConfig);
    registerHandlers();

    // init and start all the modules
    m_sealer->init(m_pbft);
    m_blockSync->init();
    m_pbft->init();

    m_sealer->start();
    m_blockSync->start();
    m_pbft->start();
}


void PBFTServiceServer::registerHandlers()
{
    // register handlers for the consensus to interact with the sealer
    auto sealer = m_sealer;
    m_pbft->registerSealProposalNotifier(
        [sealer](size_t _proposalIndex, size_t _proposalEndIndex, size_t _maxTxsToSeal,
            std::function<void(bcos::Error::Ptr)> _onRecvResponse) {
            sealer->asyncNotifySealProposal(
                _proposalIndex, _proposalEndIndex, _maxTxsToSeal, _onRecvResponse);
        });

    // the consensus module notify the latest blockNumber to the sealer
    m_pbft->registerStateNotifier([sealer](bcos::protocol::BlockNumber _blockNumber) {
        sealer->asyncNoteLatestBlockNumber(_blockNumber);
    });

    // the consensus moudle notify new block to the sync module
    auto blockSync = m_blockSync;
    m_pbft->registerNewBlockNotifier([blockSync](bcos::ledger::LedgerConfig::Ptr _ledgerConfig,
                                         std::function<void(bcos::Error::Ptr)> _onRecv) {
        blockSync->asyncNotifyNewBlock(_ledgerConfig, _onRecv);
    });
}

void PBFTServiceServer::createTxPool(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    PBFTSERVICE_LOG(INFO) << LOG_DESC("createTxPool");
    auto txpoolProxy = Application::getCommunicator()->stringToProxy<bcostars::TxPoolServicePrx>(
        getProxyDesc(TXPOOL_SERVICE_NAME));
    m_txpool = std::make_shared<bcostars::TxPoolServiceClient>(
        txpoolProxy, m_protocolInitializer->cryptoSuite());
    PBFTSERVICE_LOG(INFO) << LOG_DESC("create TxPool client success");
}

void PBFTServiceServer::createSealer(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    PBFTSERVICE_LOG(INFO) << LOG_DESC("createSealer");
    // create sealer
    auto sealerFactory = std::make_shared<SealerFactory>(
        m_protocolInitializer->blockFactory(), m_txpool, _nodeConfig->minSealTime());
    m_sealer = sealerFactory->createSealer();
    PBFTSERVICE_LOG(INFO) << LOG_DESC("createSealer success");
}

void PBFTServiceServer::createPBFT(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    PBFTSERVICE_LOG(INFO) << LOG_DESC("createPBFT");
    auto keyPair = m_protocolInitializer->keyPair();
    // create pbft
    auto pbftFactory = std::make_shared<PBFTFactory>(m_protocolInitializer->cryptoSuite(),
        m_protocolInitializer->keyPair(), m_frontService, m_storage, m_ledger, m_txpool,
        m_dispatcher, m_protocolInitializer->blockFactory(),
        m_protocolInitializer->txResultFactory());
    m_pbft = pbftFactory->createPBFT();
    auto pbftConfig = m_pbft->pbftEngine()->pbftConfig();
    pbftConfig->setCheckPointTimeoutInterval(_nodeConfig->checkPointTimeoutInterval());
    PBFTSERVICE_LOG(INFO) << LOG_DESC("createPBFT success");
}

void PBFTServiceServer::createBlockSync(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    PBFTSERVICE_LOG(INFO) << LOG_DESC("createBlockSync");
    // create sync
    auto keyPair = m_protocolInitializer->keyPair();
    auto blockSyncFactory = std::make_shared<BlockSyncFactory>(keyPair->publicKey(),
        m_protocolInitializer->blockFactory(), m_protocolInitializer->txResultFactory(), m_ledger,
        m_txpool, m_frontService, m_dispatcher, m_pbft);
    m_blockSync = blockSyncFactory->createBlockSync();
    PBFTSERVICE_LOG(INFO) << LOG_DESC("createBlockSync success");
}

Error PBFTServiceServer::asyncCheckBlock(
    const Block& _block, tars::Bool&, tars::TarsCurrentPtr _current)
{
    auto blockFactory = m_protocolInitializer->blockFactory();
    _current->setResponse(false);
    auto block = std::make_shared<bcostars::protocol::BlockImpl>(
        blockFactory->transactionFactory(), blockFactory->receiptFactory());
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

bcostars::Error PBFTServiceServer::asyncNotifyBlockSyncMessage(std::string const& _uuid,
    const vector<tars::UInt8>& _nodeId, const vector<tars::UInt8>& _data,
    tars::TarsCurrentPtr _current)
{
    _current->setResponse(false);
    auto nodeId = m_keyFactory->createKey(_nodeId);
    m_blockSync->asyncNotifyBlockSyncMessage(
        nullptr, _uuid, nodeId, bcos::ref(_data), [_current](bcos::Error::Ptr _error) {
            async_response_asyncNotifyBlockSyncMessage(_current, toTarsError(_error));
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

bcostars::Error PBFTServiceServer::asyncGetSyncInfo(
    std::string& _syncInfo, tars::TarsCurrentPtr _current)
{
    _current->setResponse(false);
    m_blockSync->asyncGetSyncInfo(
        [_current](bcos::Error::Ptr _error, std::string const& _syncInfo) {
            async_response_asyncGetSyncInfo(_current, toTarsError(_error), _syncInfo);
        });
}