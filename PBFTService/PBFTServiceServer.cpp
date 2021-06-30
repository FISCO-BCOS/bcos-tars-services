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
    auto iniConfigPath = ServerConfig::BasePath + "config.ini";
    nodeConfig->loadConfig(iniConfigPath);

    auto genesisConfigPath = ServerConfig::BasePath + "config.genesis";
    nodeConfig->loadGenesisConfig(genesisConfigPath);

    m_protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
    m_protocolInitializer->init(nodeConfig);
    auto privateKeyPath = ServerConfig::BasePath + "node.pem";
    m_protocolInitializer->loadKeyPair(privateKeyPath);

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
    auto ledger =
        std::make_shared<bcos::ledger::Ledger>(m_protocolInitializer->blockFactory(), m_storage);
    m_ledger = ledger;
    // write the genesis block through ledger
    ledger->buildGenesisBlock(
        nodeConfig->ledgerConfig(), nodeConfig->txGasLimit(), nodeConfig->genesisData());

    // TODO: create the txpool client only
    createTxPool(nodeConfig);

    createSealer(nodeConfig);
    createPBFT(nodeConfig);
    createBlockSync(nodeConfig);
    registerHandlers();

    // init and start all the modules
    m_txpool->init();
    m_sealer->init(m_pbft);
    m_blockSync->init();
    m_pbft->init();

    m_txpool->start();
    m_sealer->start();
    m_blockSync->start();
    m_pbft->start();
}


void PBFTServiceServer::registerHandlers()
{
    // register handlers for the txpool to interact with the sealer
    std::weak_ptr<SealerInterface> weakedSealer = m_sealer;
    m_txpool->registerUnsealedTxsNotifier(
        [weakedSealer](size_t _unsealedTxsSize, std::function<void(bcos::Error::Ptr)> _onRecv) {
            try
            {
                auto sealer = weakedSealer.lock();
                if (!sealer)
                {
                    return;
                }
                sealer->asyncNoteUnSealedTxsSize(_unsealedTxsSize, _onRecv);
            }
            catch (std::exception const& e)
            {
                PBFTSERVICE_LOG(WARNING)
                    << LOG_DESC("call UnsealedTxsNotifier to the sealer exception")
                    << LOG_KV("error", boost::diagnostic_information(e));
            }
        });

    // register handlers for the consensus to interact with the sealer
    m_pbft->registerSealProposalNotifier(
        [weakedSealer](size_t _proposalIndex, size_t _proposalEndIndex, size_t _maxTxsToSeal,
            std::function<void(bcos::Error::Ptr)> _onRecvResponse) {
            try
            {
                auto sealer = weakedSealer.lock();
                if (!sealer)
                {
                    return;
                }
                sealer->asyncNotifySealProposal(
                    _proposalIndex, _proposalEndIndex, _maxTxsToSeal, _onRecvResponse);
            }
            catch (std::exception const& e)
            {
                PBFTSERVICE_LOG(WARNING) << LOG_DESC("call notify proposal sealing exception")
                                         << LOG_KV("error", boost::diagnostic_information(e));
            }
        });

    // the consensus module notify the latest blockNumber to the sealer
    m_pbft->registerStateNotifier([weakedSealer](bcos::protocol::BlockNumber _blockNumber) {
        try
        {
            auto sealer = weakedSealer.lock();
            if (!sealer)
            {
                return;
            }
            sealer->asyncNoteLatestBlockNumber(_blockNumber);
        }
        catch (std::exception const& e)
        {
            PBFTSERVICE_LOG(WARNING)
                << LOG_DESC("call notify the latest block number to the sealer exception")
                << LOG_KV("error", boost::diagnostic_information(e));
        }
    });

    // the consensus moudle notify new block to the sync module
    std::weak_ptr<BlockSyncInterface> weakedSync = m_blockSync;
    m_pbft->registerNewBlockNotifier([weakedSync](bcos::ledger::LedgerConfig::Ptr _ledgerConfig,
                                         std::function<void(bcos::Error::Ptr)> _onRecv) {
        try
        {
            auto sync = weakedSync.lock();
            if (!sync)
            {
                return;
            }
            sync->asyncNotifyNewBlock(_ledgerConfig, _onRecv);
        }
        catch (std::exception const& e)
        {
            PBFTSERVICE_LOG(WARNING)
                << LOG_DESC("call notify the latest block to the sync module exception")
                << LOG_KV("error", boost::diagnostic_information(e));
        }
    });
}

void PBFTServiceServer::createTxPool(bcos::tool::NodeConfig::Ptr _nodeConfig)
{
    PBFTSERVICE_LOG(INFO) << LOG_DESC("createTxPool");
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
    PBFTSERVICE_LOG(INFO) << LOG_DESC("createTxPool success");
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