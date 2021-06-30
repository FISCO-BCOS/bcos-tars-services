#pragma once

#include "../Common/ErrorConverter.h"
#include "../protocols/TransactionImpl.h"
#include "../protocols/TransactionSubmitResultImpl.h"
#include "Common.h"
#include "TxPoolService.h"
#include "bcos-txpool/TxPool.h"
#include "interfaces/crypto/CommonType.h"
#include "interfaces/crypto/Hash.h"
#include "interfaces/crypto/KeyInterface.h"
#include "libutilities/Common.h"
#include "libutilities/FixedBytes.h"
#include <memory>

namespace bcostars
{
class TxPoolServiceServer : public bcostars::TxPoolService
{
public:
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

private:
    bcos::txpool::TxPool::Ptr m_txpool;
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
}  // namespace bcostars