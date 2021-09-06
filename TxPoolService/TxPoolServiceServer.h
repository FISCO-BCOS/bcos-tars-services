#pragma once

#include "../Common/ErrorConverter.h"
#include "../Common/TarsUtils.h"
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
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <bcos-ledger/libledger/Ledger.h>
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
        try
        {
            std::call_once(m_initFlag, [this]() {
                init();
                m_running = true;
            });
        }
        catch (std::exception const& e)
        {
            TLOGERROR("init the txpoolService exceptioned"
                      << LOG_KV("error", boost::diagnostic_information(e)) << std::endl);
            exit(0);
        }
    }

    void init()
    {
        // load the configuration for txpool
        auto configPath = ServerConfig::BasePath + "config.ini";
        boost::property_tree::ptree pt;
        boost::property_tree::read_ini(configPath, pt);
        m_logInitializer = std::make_shared<bcos::BoostLogInitializer>();
        // set the boost log into the tars log directory
        m_logInitializer->setLogPath(getLogPath());
        m_logInitializer->initLog(pt);
        TLOGINFO(LOG_DESC("TxPoolService initLog success") << std::endl);

        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("load nodeConfig");
        auto nodeConfig = std::make_shared<bcos::tool::NodeConfig>();
        nodeConfig->loadConfig(configPath);
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("load nodeConfig success");

        // create the protocolInitializer
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("load protocol and nodeID");
        auto protocolInitializer = std::make_shared<bcos::initializer::ProtocolInitializer>();
        protocolInitializer->init(nodeConfig);
        auto privateKeyPath = ServerConfig::BasePath + "node.pem";
        protocolInitializer->loadKeyPair(privateKeyPath);
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("load protocol and nodeID success")
                                << LOG_KV("nodeID",
                                       protocolInitializer->keyPair()->publicKey()->shortHex());

        m_cryptoSuite = protocolInitializer->cryptoSuite();

        // create the storage client
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("create the storage client");
        auto storageProxy =
            Application::getCommunicator()->stringToProxy<bcostars::StorageServicePrx>(
                getProxyDesc(STORAGE_SERVICE_NAME));
        auto storage = std::make_shared<bcostars::StorageServiceClient>(storageProxy);
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("create the storage client success");

        // create the ledger
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("create the ledger");
        m_ledger =
            std::make_shared<bcos::ledger::Ledger>(protocolInitializer->blockFactory(), storage);
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("create the ledger success");

        // create the frontService client
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("create the frontService client");
        auto frontServiceProxy =
            Application::getCommunicator()->stringToProxy<bcostars::FrontServicePrx>(
                getProxyDesc(FRONT_SERVICE_NAME));
        auto frontService = std::make_shared<bcostars::FrontServiceClient>(
            frontServiceProxy, protocolInitializer->keyFactory());
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("create the frontService client success");

        // create txpoolFactory
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("create the txpool");
        auto txpoolFactory = std::make_shared<bcos::txpool::TxPoolFactory>(
            protocolInitializer->keyPair()->publicKey(), protocolInitializer->cryptoSuite(),
            protocolInitializer->txResultFactory(), protocolInitializer->blockFactory(),
            frontService, m_ledger, nodeConfig->groupId(), nodeConfig->chainId(),
            nodeConfig->blockLimit());

        auto txpool = txpoolFactory->createTxPool();
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("create the txpool success");
        m_txpool = txpool;

        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("load the txpool config");
        auto txpoolConfig = txpool->txpoolConfig();
        txpoolConfig->setPoolLimit(nodeConfig->txpoolLimit());
        txpoolConfig->setNotifierWorkerNum(nodeConfig->notifyWorkerNum());
        txpoolConfig->setVerifyWorkerNum(nodeConfig->verifierWorkerNum());
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("load the txpool config success");

        // register handlers for the txpool to interact with the sealer
        auto pbftProxy = Application::getCommunicator()->stringToProxy<PBFTServicePrx>(
            getProxyDesc(PBFT_SERVICE_NAME));
        auto sealer = std::make_shared<PBFTServiceClient>(pbftProxy);
        m_txpool->registerUnsealedTxsNotifier(
            [sealer](size_t _unsealedTxsSize, std::function<void(bcos::Error::Ptr)> _onRecv) {
                try
                {
                    sealer->asyncNoteUnSealedTxsSize(_unsealedTxsSize, _onRecv);
                }
                catch (std::exception const& e)
                {
                    TXPOOLSERVICE_LOG(WARNING)
                        << LOG_DESC("call UnsealedTxsNotifier to the sealer exception")
                        << LOG_KV("error", boost::diagnostic_information(e));
                }
            });
        // init and start the txpool
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("init and start the txpool");
        txpool->init();
        m_txpool->start();
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("init and start the txpool success");
    }

    void destroy() override
    {
        if (!m_running)
        {
            TXPOOLSERVICE_LOG(WARNING) << LOG_DESC("The txpoolService has already stopped!");
            return;
        }
        m_running = false;
        TXPOOLSERVICE_LOG(INFO) << LOG_DESC("Stop the txpoolService");
        if (m_txpool)
        {
            m_txpool->stop();
        }
        if (m_ledger)
        {
            m_ledger->stop();
        }
        TLOGINFO(LOG_DESC("[TXPOOLSERVICE] Stop the txpoolService success") << std::endl);
    }

    bcostars::Error asyncFillBlock(const vector<vector<tars::Char>>& txHashs,
        vector<bcostars::Transaction>& filled, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);
        auto hashList = std::make_shared<std::vector<bcos::crypto::HashType>>();
        for (auto const& hashData : txHashs)
        {
            hashList->push_back(bcos::crypto::HashType(
                reinterpret_cast<const bcos::byte*>(hashData.data()), hashData.size()));
        }

        m_txpool->asyncFillBlock(hashList, [current](bcos::Error::Ptr error,
                                               bcos::protocol::TransactionsPtr txs) {
            std::vector<bcostars::Transaction> txList;
            if (error)
            {
                async_response_asyncFillBlock(current, toTarsError(error), txList);
                TXPOOLSERVICE_LOG(WARNING)
                    << LOG_DESC("asyncFillBlock failed") << LOG_KV("code", error->errorCode())
                    << LOG_KV("msg", error->errorMessage());
                return;
            }
            for (auto tx : *txs)
            {
                txList.push_back(
                    std::dynamic_pointer_cast<bcostars::protocol::TransactionImpl>(tx)->inner());
            }

            async_response_asyncFillBlock(current, toTarsError(error), txList);
        });

        return bcostars::Error();
    }

    bcostars::Error asyncMarkTxs(const vector<vector<tars::Char>>& txHashs, tars::Bool sealedFlag,
        tars::Int64 _batchId, const vector<tars::Char>& _batchHash,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);
        auto hashList = std::make_shared<std::vector<bcos::crypto::HashType>>();
        for (auto hashData : txHashs)
        {
            hashList->push_back(bcos::crypto::HashType(
                reinterpret_cast<const bcos::byte*>(hashData.data()), hashData.size()));
        }
        auto batchHash = bcos::crypto::HashType(
            reinterpret_cast<const bcos::byte*>(_batchHash.data()), _batchHash.size());
        m_txpool->asyncMarkTxs(
            hashList, sealedFlag, _batchId, batchHash, [current](bcos::Error::Ptr error) {
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
        const vector<tars::Char>& nodeID, const vector<tars::Char>& data,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosNodeID =
            m_cryptoSuite->keyFactory()->createKey(bcos::bytes(nodeID.begin(), nodeID.end()));

        m_txpool->asyncNotifyTxsSyncMessage(toBcosError(error), id, bcosNodeID,
            bcos::bytesConstRef(reinterpret_cast<const bcos::byte*>(data.data()), data.size()),
            [current](bcos::Error::Ptr error) {
                async_response_asyncNotifyTxsSyncMessage(current, toTarsError(error));
            });

        return bcostars::Error();
    }

    bcostars::Error asyncSealTxs(tars::Int64 txsLimit, const vector<vector<tars::Char>>& avoidTxs,
        vector<vector<tars::Char>>& return1, vector<vector<tars::Char>>& return2,
        tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        auto bcosAvoidTxs = std::make_shared<bcos::txpool::TxsHashSet>();
        for (auto tx : avoidTxs)
        {
            bcosAvoidTxs->insert(bcos::crypto::HashType(bcos::bytes(tx.begin(), tx.end())));
        }

        m_txpool->asyncSealTxs(txsLimit, bcosAvoidTxs,
            [current](bcos::Error::Ptr error, bcos::crypto::HashListPtr list1,
                bcos::crypto::HashListPtr list2) {
                if (error)
                {
                    TXPOOLSERVICE_LOG(WARNING)
                        << LOG_DESC("asyncSealTxs failed") << LOG_KV("code", error->errorCode())
                        << LOG_KV("msg", error->errorMessage());
                    async_response_asyncSealTxs(current, toTarsError(error),
                        vector<vector<tars::Char>>(), vector<vector<tars::Char>>());
                    return;
                }
                vector<vector<tars::Char>> returnList1;
                for (auto hash1 : *list1)
                {
                    returnList1.emplace_back(hash1.begin(), hash1.end());
                }

                vector<vector<tars::Char>> returnList2;
                for (auto hash2 : *list2)
                {
                    returnList2.emplace_back(hash2.begin(), hash2.end());
                }

                async_response_asyncSealTxs(current, toTarsError(error), returnList1, returnList2);
            });

        return bcostars::Error();
    }

    bcostars::Error asyncSubmit(const vector<tars::Char>& tx,
        bcostars::TransactionSubmitResult& result, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);
        auto dataPtr = std::make_shared<bcos::bytes>(tx.begin(), tx.end());
        m_txpool->asyncSubmit(dataPtr, [current](bcos::Error::Ptr error,
                                           bcos::protocol::TransactionSubmitResult::Ptr result) {
            async_response_asyncSubmit(current, toTarsError(error),
                std::dynamic_pointer_cast<bcostars::protocol::TransactionSubmitResultImpl>(result)
                    ->inner());
        });

        return bcostars::Error();
    }

    bcostars::Error asyncVerifyBlock(const vector<tars::Char>& generatedNodeID,
        const vector<tars::Char>& block, tars::Bool& result, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        bcos::crypto::PublicPtr pk = m_cryptoSuite->keyFactory()->createKey(
            bcos::bytesConstRef((const bcos::byte*)generatedNodeID.data(), generatedNodeID.size()));
        m_txpool->asyncVerifyBlock(pk,
            bcos::bytesConstRef((const bcos::byte*)block.data(), block.size()),
            [current](bcos::Error::Ptr error, bool result) {
                async_response_asyncVerifyBlock(current, toTarsError(error), result);
            });

        return bcostars::Error();
    }

    bcostars::Error notifyConnectedNodes(
        const vector<vector<tars::Char>>& connectedNodes, tars::TarsCurrentPtr current) override
    {
        current->setResponse(false);

        bcos::crypto::NodeIDSet bcosNodeIDSet;
        for (auto const& it : connectedNodes)
        {
            bcosNodeIDSet.insert(m_cryptoSuite->keyFactory()->createKey(
                bcos::bytesConstRef((const bcos::byte*)it.data(), it.size())));
        }

        m_txpool->transactionSync()->config()->notifyConnectedNodes(
            bcosNodeIDSet, [current](bcos::Error::Ptr error) {
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
                m_cryptoSuite->keyFactory()->createKey(
                    bcos::bytesConstRef((const bcos::byte*)it.nodeID.data(), it.nodeID.size())),
                it.weight);
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
                m_cryptoSuite->keyFactory()->createKey(
                    bcos::bytesConstRef((const bcos::byte*)it.nodeID.data(), it.nodeID.size())),
                it.weight);
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
        return bcostars::Error();
    }

    bcostars::Error asyncResetTxPool(tars::TarsCurrentPtr _current) override
    {
        _current->setResponse(false);
        m_txpool->asyncResetTxPool([_current](bcos::Error::Ptr _error) {
            async_response_asyncResetTxPool(_current, toTarsError(_error));
        });
        return bcostars::Error();
    }

private:
    static std::once_flag m_initFlag;
    static bcos::txpool::TxPool::Ptr m_txpool;
    static std::shared_ptr<bcos::ledger::Ledger> m_ledger;
    static bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
    static std::atomic_bool m_running;
    static bcos::BoostLogInitializer::Ptr m_logInitializer;
};
}  // namespace bcostars