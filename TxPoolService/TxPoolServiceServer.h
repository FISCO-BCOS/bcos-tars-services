#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/ProxyDesc.h"
#include "../FrontService/FrontServiceClient.h"
#include "../PBFTService/PBFTServiceClient.h"
#include "../StorageService/StorageServiceClient.h"
#include "../libinitializer/ProtocolInitializer.h"
#include "../protocols/TransactionImpl.h"
#include "../protocols/TransactionSubmitResultImpl.h"
#include "Common.h"
#include "TxPoolService.h"
#include "bcos-txpool/TxPool.h"
#include "interfaces/consensus/ConsensusNode.h"
#include "interfaces/crypto/CommonType.h"
#include "interfaces/crypto/Hash.h"
#include "interfaces/crypto/KeyInterface.h"
#include "libutilities/Common.h"
#include "libutilities/FixedBytes.h"
#include "servant/Servant.h"
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-ledger/ledger/Ledger.h>
#include <bcos-txpool/TxPoolFactory.h>
#include <memory>

#define TXPOOLSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[TXPOOLSERVICE]"
namespace bcostars
{
class TxPoolServiceServer : public bcostars::TxPoolService
{
public:
    void initialize() override
    {
        // load the configuration for txpool
        auto configPath = ServerConfig::BasePath + "config.ini";
        auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>();
        nodeConfig->loadConfig(configPath);

        // create the protocolInitializer
        auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
        protocolInitializer->init(nodeConfig);
        auto privateKeyPath = ServerConfig::BasePath + "node.pem";
        protocolInitializer->loadKeyPair(privateKeyPath);

        // create the storage client
        auto storageProxy =
            Application::getCommunicator()->stringToProxy<bcostars::StorageServicePrx>(
                getProxyDesc("StorageServiceObj"));
        auto storage = std::make_shared<bcostars::StorageServiceClient>(storageProxy);

        // create the ledger
        auto ledger =
            std::make_shared<bcos::ledger::Ledger>(protocolInitializer->blockFactory(), storage);

        // create the frontService client
        auto frontServiceProxy =
            Application::getCommunicator()->stringToProxy<bcostars::FrontServicePrx>(
                getProxyDesc("FrontServiceObj"));
        auto frontService = std::make_shared<bcostars::FrontServiceClient>(
            frontServiceProxy, protocolInitializer->keyFactory());


        // create txpoolFactory
        auto txpoolFactory = std::make_shared<bcos::txpool::TxPoolFactory>(
            protocolInitializer->keyPair()->publicKey(), protocolInitializer->cryptoSuite(),
            protocolInitializer->txResultFactory(), protocolInitializer->blockFactory(),
            frontService, ledger, nodeConfig->groupId(), nodeConfig->chainId(),
            nodeConfig->blockLimit());

        auto txpool = txpoolFactory->createTxPool();
        m_txpool = txpool;
        auto txpoolConfig = txpool->txpoolConfig();
        txpoolConfig->setPoolLimit(nodeConfig->txpoolLimit());
        txpoolConfig->setNotifierWorkerNum(nodeConfig->notifyWorkerNum());
        txpoolConfig->setVerifyWorkerNum(nodeConfig->verifierWorkerNum());

        // register handlers for the txpool to interact with the sealer
        auto pbftProxy = Application::getCommunicator()->stringToProxy<PBFTServicePrx>(
            getProxyDesc("PBFTServiceObj"));
        auto pbft = std::make_shared<PBFTServiceClient>(pbftProxy);
        m_txpool->registerUnsealedTxsNotifier(
            [pbft](size_t _unsealedTxsSize, std::function<void(bcos::Error::Ptr)> _onRecv) {
                try
                {
                    pbft->asyncNoteUnSealedTxsSize(_unsealedTxsSize, _onRecv);
                }
                catch (std::exception const& e)
                {
                    TXPOOLSERVICE_LOG(WARNING)
                        << LOG_DESC("call UnsealedTxsNotifier to the sealer exception")
                        << LOG_KV("error", boost::diagnostic_information(e));
                }
            });
        // init and start the txpool
        txpool->init();
        m_txpool->start();
    }

    void destroy() override {}

    bcostars::Error asyncFillBlock(const vector<vector<tars::UInt8>>& txHashs,
        vector<bcostars::Transaction>& filled, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);
        auto hashList = std::make_shared<std::vector<bcos::crypto::HashType>>();
        for (auto hashData : txHashs)
        {
            hashList->push_back(bcos::crypto::HashType(hashData));
        }

        m_txpool->asyncFillBlock(hashList, [current](bcos::Error::Ptr error,
                                               bcos::protocol::TransactionsPtr txs) {
            std::vector<bcostars::Transaction> txList;
            for (auto tx : *txs)
            {
                txList.push_back(
                    std::dynamic_pointer_cast<bcostars::protocol::TransactionImpl>(tx)->inner());
            }

            async_response_asyncFillBlock(current, toTarsError(error), txList);
        });

        return bcostars::Error();
    }

    bcostars::Error asyncMarkTxs(const vector<vector<tars::UInt8>>& txHashs, tars::Bool sealedFlag,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);
        auto hashList = std::make_shared<std::vector<bcos::crypto::HashType>>();
        for (auto hashData : txHashs)
        {
            hashList->push_back(bcos::crypto::HashType(hashData));
        }

        m_txpool->asyncMarkTxs(hashList, sealedFlag, [current](bcos::Error::Ptr error) {
            async_response_asyncMarkTxs(current, toTarsError(error));
        });
        return bcostars::Error();
    }

    bcostars::Error asyncNotifyBlockResult(tars::Int64 blockNumber,
        const vector<bcostars::TransactionSubmitResult>& result,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosResultList = std::make_shared<bcos::protocol::TransactionSubmitResults>();
        for (auto tarsResult : result)
        {
            auto bcosResult =
                std::make_shared<bcostars::protocol::TransactionSubmitResultImpl>(m_cryptoSuite);
            bcosResult->setInner(tarsResult);
            bcosResultList->push_back(bcosResult);
        }

        m_txpool->asyncNotifyBlockResult(
            blockNumber, bcosResultList, [current](bcos::Error::Ptr error) {
                async_response_asyncNotifyBlockResult(current, toTarsError(error));
            });
        return bcostars::Error();
    }

    bcostars::Error asyncNotifyTxsSyncMessage(const bcostars::Error& error, const std::string& id,
        const vector<tars::UInt8>& nodeID, const vector<tars::UInt8>& data,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosNodeID = m_cryptoSuite->keyFactory()->createKey(nodeID);

        m_txpool->asyncNotifyTxsSyncMessage(
            toBcosError(error), id, bcosNodeID, bcos::ref(data), [current](bcos::Error::Ptr error) {
                async_response_asyncNotifyTxsSyncMessage(current, toTarsError(error));
            });

        return bcostars::Error();
    }

    bcostars::Error asyncSealTxs(tars::Int64 txsLimit, const vector<vector<tars::UInt8>>& avoidTxs,
        vector<vector<tars::UInt8>>& return1, vector<vector<tars::UInt8>>& return2,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosAvoidTxs = std::make_shared<bcos::txpool::TxsHashSet>();
        for (auto tx : avoidTxs)
        {
            bcosAvoidTxs->insert(bcos::crypto::HashType(tx));
        }

        m_txpool->asyncSealTxs(txsLimit, bcosAvoidTxs,
            [current](bcos::Error::Ptr error, bcos::crypto::HashListPtr list1,
                bcos::crypto::HashListPtr list2) {
                vector<vector<tars::UInt8>> returnList1;
                for (auto hash1 : *list1)
                {
                    returnList1.emplace_back(hash1.asBytes());
                }

                vector<vector<tars::UInt8>> returnList2;
                for (auto hash2 : *list2)
                {
                    returnList2.emplace_back(hash2.asBytes());
                }

                async_response_asyncSealTxs(current, toTarsError(error), returnList1, returnList2);
            });

        return bcostars::Error();
    }

    bcostars::Error asyncSubmit(const vector<tars::UInt8>& tx,
        bcostars::TransactionSubmitResult& result, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);
        auto dataPtr = std::make_shared<bcos::bytes>(tx);
        m_txpool->asyncSubmit(dataPtr, [current](bcos::Error::Ptr error,
                                           bcos::protocol::TransactionSubmitResult::Ptr result) {
            async_response_asyncSubmit(current, toTarsError(error),
                std::dynamic_pointer_cast<bcostars::protocol::TransactionSubmitResultImpl>(result)
                    ->inner());
        });

        return bcostars::Error();
    }

    bcostars::Error asyncVerifyBlock(const vector<tars::UInt8>& generatedNodeID,
        const vector<tars::UInt8>& block, tars::Bool& result, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        bcos::crypto::PublicPtr pk = m_cryptoSuite->keyFactory()->createKey(generatedNodeID);
        m_txpool->asyncVerifyBlock(
            pk, bcos::ref(block), [current](bcos::Error::Ptr error, bool result) {
                async_response_asyncVerifyBlock(current, toTarsError(error), result);
            });

        return bcostars::Error();
    }

    bcostars::Error notifyConnectedNodes(
        const vector<vector<tars::UInt8>>& connectedNodes, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        bcos::crypto::NodeIDSet bcosNodeIDSet;
        for (auto const& it : connectedNodes)
        {
            bcosNodeIDSet.insert(m_cryptoSuite->keyFactory()->createKey(it));
        }

        m_txpool->notifyConnectedNodes(bcosNodeIDSet, [current](bcos::Error::Ptr error) {
            async_response_notifyConnectedNodes(current, toTarsError(error));
        });

        return bcostars::Error();
    }

    bcostars::Error notifyConsensusNodeList(
        const vector<bcostars::ConsensusNode>& consensusNodeList,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        bcos::consensus::ConsensusNodeList bcosNodeList;
        for (auto const& it : consensusNodeList)
        {
            bcos::consensus::ConsensusNode node(
                m_cryptoSuite->keyFactory()->createKey(it.nodeID), it.weight);
        }

        m_txpool->notifyConsensusNodeList(bcosNodeList, [current](bcos::Error::Ptr error) {
            async_response_notifyConsensusNodeList(current, toTarsError(error));
        });

        return bcostars::Error();
    }

    bcostars::Error notifyObserverNodeList(const vector<bcostars::ConsensusNode>& observerNodeList,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        bcos::consensus::ConsensusNodeList bcosObserverNodeList;
        for (auto const& it : observerNodeList)
        {
            bcos::consensus::ConsensusNode node(
                m_cryptoSuite->keyFactory()->createKey(it.nodeID), it.weight);
        }

        m_txpool->notifyObserverNodeList(bcosObserverNodeList, [current](bcos::Error::Ptr error) {
            async_response_notifyObserverNodeList(current, toTarsError(error));
        });

        return bcostars::Error();
    }

    bcostars::Error asyncGetPendingTransactionSize(
        tars::Int64& _pendingTxsSize, tars::TarsCurrentPtr _current) override
    {
        _current->setResponse(false);
        m_txpool->asyncGetPendingTransactionSize([_current](
                                                     bcos::Error::Ptr _error, size_t _txsSize) {
            async_response_asyncGetPendingTransactionSize(_current, toTarsError(_error), _txsSize);
        });
    }

private:
    bcos::txpool::TxPool::Ptr m_txpool;
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
}  // namespace bcostars