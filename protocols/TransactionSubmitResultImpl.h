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

    uint32_t status() const override { return m_inner->status; }
    bcos::protocol::TransactionReceipt::Ptr receipt() const override
    {
        auto inner = std::const_pointer_cast<bcostars::TransactionSubmitResult>(m_inner);
        auto receipt = std::make_shared<bcostars::protocol::TransactionReceiptImpl>(
            m_cryptoSuite, [m_inner = std::move(inner)]() mutable { return &m_inner->receipt; });

        return receipt;
    }
    bcos::crypto::HashType const& txHash() const override
    {
        if (!m_inner->txHash.empty())
        {
            return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner->txHash.data()));
        }
        return emptyHash;
    }
    bcos::bytesConstRef from() const override
    {
        return bcos::bytesConstRef(
            reinterpret_cast<const bcos::byte*>(m_inner->from.data()), m_inner->from.size());
    }
    bcos::crypto::HashType const& blockHash() const override
    {
        if (!m_inner->blockHash.empty())
        {
            return *(reinterpret_cast<const bcos::crypto::HashType*>(m_inner->blockHash.data()));
        }
        return emptyHash;
    }
    bcos::bytesConstRef to() const override
    {
        return bcos::bytesConstRef(
            reinterpret_cast<const bcos::byte*>(m_inner->to.data()), m_inner->to.size());
    }
    int64_t transactionIndex() const override { return m_inner->transactionIndex; }
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

    bcos::protocol::TransactionSubmitResult::Ptr createTxSubmitResult(
        bcos::protocol::TransactionReceipt::Ptr _receipt, bcos::crypto::HashType _txHash,
        int64_t _txIndex, bcos::crypto::HashType _blockHash, bcos::bytesConstRef _sender,
        bcos::bytesConstRef _to) override
    {
        auto result = std::make_shared<TransactionSubmitResultImpl>(m_cryptoSuite);
        result->m_inner->receipt =
            std::dynamic_pointer_cast<bcostars::protocol::TransactionReceiptImpl>(_receipt)
                ->inner();
        result->m_inner->txHash.assign(_txHash.data(), _txHash.data() + _txHash.size);
        result->m_inner->transactionIndex = _txIndex;
        result->m_inner->blockHash.assign(_blockHash.begin(), _blockHash.end());
        result->m_inner->from.assign(_sender.begin(), _sender.end());
        result->m_inner->to.assign(_to.begin(), _to.end());

        return result;
    }

    bcos::protocol::TransactionSubmitResult::Ptr createTxSubmitResult(
        bcos::protocol::TransactionReceipt::Ptr _receipt, bcos::protocol::Transaction::Ptr _tx,
        int64_t _txIndex, bcos::protocol::BlockHeader::Ptr _blockHeader) override
    {
        auto result = std::make_shared<TransactionSubmitResultImpl>(m_cryptoSuite);
        auto txHash = _tx->hash();
        result->m_inner->txHash.assign(txHash.begin(), txHash.end());
        auto to = _tx->to();
        result->m_inner->to.assign(to.begin(), to.end());

        result->m_inner->transactionIndex = _txIndex;
        auto blockHash = _blockHeader->hash();
        result->m_inner->blockHash.assign(blockHash.begin(), blockHash.end());

        return result;
    }

private:
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
}  // namespace protocol
}  // namespace bcostars