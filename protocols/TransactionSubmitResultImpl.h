#pragma once

#include "Common.h"
#include "TransactionReceipt.h"
#include "TransactionReceiptImpl.h"
#include "TransactionSubmitResult.h"
#include "interfaces/crypto/CommonType.h"
#include "libutilities/Common.h"
#include <bcos-framework/interfaces/protocol/TransactionSubmitResult.h>
#include <bcos-framework/interfaces/protocol/TransactionSubmitResultFactory.h>
#include <boost/lexical_cast.hpp>
#include <memory>

namespace bcostars
{
namespace protocol
{
class TransactionSubmitResultImpl : public bcos::protocol::TransactionSubmitResult
{
public:
    friend class TransactionSubmitResultFactoryImpl;

    TransactionSubmitResultImpl() = delete;

    TransactionSubmitResultImpl(bcos::crypto::CryptoSuite::Ptr cryptoSuite)
      : bcos::protocol::TransactionSubmitResult(),
        m_cryptoSuite(cryptoSuite),
        m_inner(std::make_shared<bcostars::TransactionSubmitResult>())
    {}

    uint32_t status() const override { return m_inner.status; }
    bcos::crypto::HashType const& txHash() const override
    {
        if (!m_inner->txHash.empty())
        {
            return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner->txHash.data()));
        }
        return emptyHash;
    }
    bcos::crypto::HashType const& blockHash() const override
    {
        if (!m_inner->blockHash.empty())
        {
            return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner->blockHash.data()));
        }
        return emptyHash;
    }

    int64_t transactionIndex() const override { return m_inner.transactionIndex; }
    void setNonce(bcos::protocol::NonceType const& _nonce) override
    {
        m_inner->nonce = boost::lexical_cast<std::string>(_nonce);
    }
    bcos::protocol::NonceType const& nonce() override
    {
        if (!m_inner->nonce.empty())
        {
            m_nonce = boost::lexical_cast<bcos::protocol::NonceType>(m_inner->nonce);
        }

        return m_nonce;
    }


    bcostars::TransactionSubmitResult const& inner() { return *m_inner; }
    void setInner(const bcostars::TransactionSubmitResult& result) { *m_inner = result; }
    void setInner(bcostars::TransactionSubmitResult&& result) { *m_inner = std::move(result); }

private:
    mutable bcos::protocol::NonceType m_nonce;
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
    std::shared_ptr<bcostars::TransactionSubmitResult> m_inner;
};

class TransactionSubmitResultFactoryImpl : public bcos::protocol::TransactionSubmitResultFactory
{
public:
    bcos::protocol::TransactionSubmitResult::Ptr createTxSubmitResult(
        bcos::protocol::BlockHeader::Ptr _blockHeader,
        bcos::crypto::HashType const& _txHash) override
    {
        auto result = std::make_shared<TransactionSubmitResultImpl>(m_cryptoSuite);
        result->m_inner->txHash.assign(_txHash.begin(), _txHash.end());
        auto blockHash = _blockHeader->hash();
        result->m_inner->blockHash.assign(blockHash.begin(), blockHash.end());

        return result;
    }

    bcos::protocol::TransactionSubmitResult::Ptr createTxSubmitResult(
        bcos::crypto::HashType const& _txHash, int32_t _status) override
    {
        auto result = std::make_shared<TransactionSubmitResultImpl>(m_cryptoSuite);
        result->m_inner->txHash.assign(_txHash.begin(), _txHash.end());
        result->m_inner->status = _status;

        return result;
    }

private:
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
}  // namespace protocol
}  // namespace bcostars