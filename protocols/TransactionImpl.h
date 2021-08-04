#pragma once

#include "Block.h"
#include "Common.h"
#include "Transaction.h"
#include "bcos-framework/libutilities/DataConvertUtility.h"
#include "interfaces/protocol/Block.h"
#include <bcos-framework/interfaces/crypto/CommonType.h>
#include <bcos-framework/interfaces/protocol/Transaction.h>
#include <bcos-framework/interfaces/protocol/TransactionFactory.h>
#include <bcos-framework/libutilities/Common.h>
#include <memory>

namespace bcostars
{
namespace protocol
{
class TransactionImpl : public bcos::protocol::Transaction
{
public:
    explicit TransactionImpl(
        bcos::crypto::CryptoSuite::Ptr _cryptoSuite, std::function<bcostars::Transaction*()> inner)
      : bcos::protocol::Transaction(_cryptoSuite), m_inner(inner)
    {}

    ~TransactionImpl() {}

    friend class TransactionFactoryImpl;

    bool operator==(const Transaction& rhs) const { return this->hash() == rhs.hash(); }

    void decode(bcos::bytesConstRef _txData) override
    {
        m_buffer.assign(_txData.begin(), _txData.end());

        tars::TarsInputStream<tars::BufferReader> input;
        input.setBuffer((const char*)m_buffer.data(), m_buffer.size());

        m_inner()->readFrom(input);
    }

    bcos::bytesConstRef encode(bool _onlyHashFields = false) const override
    {
        if (_onlyHashFields)
        {
            if (m_dataBuffer.empty())
            {
                tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;
                m_inner()->data.writeTo(output);
                output.getByteBuffer().swap(m_dataBuffer);
            }

            return bcos::ref(m_dataBuffer);
        }
        else
        {
            if (m_buffer.empty())
            {
                tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;

                auto hash = m_cryptoSuite->hash(m_dataBuffer);
                m_inner()->dataHash.assign(hash.begin(), hash.end());
                m_inner()->writeTo(output);
                output.getByteBuffer().swap(m_buffer);
            }
            return bcos::ref(m_buffer);
        }
    }

    bcos::crypto::HashType const& hash() const override
    {
        if (m_inner()->dataHash.empty())
        {
            auto buffer = encode(true);
            auto hash = m_cryptoSuite->hash(buffer);
            m_inner()->dataHash.assign(hash.begin(), hash.end());
        }

        return *(reinterpret_cast<bcos::crypto::HashType*>(m_inner()->dataHash.data()));
    }

    int32_t version() const override { return m_inner()->data.version; }
    std::string_view chainId() const override { return m_inner()->data.chainID; }
    std::string_view groupId() const override { return m_inner()->data.groupID; }
    int64_t blockLimit() const override { return m_inner()->data.blockLimit; }
    bcos::u256 const& nonce() const override
    {
        if (!m_inner()->data.nonce.empty())
        {
            m_nonce = boost::lexical_cast<bcos::u256>(m_inner()->data.nonce);
        }
        return m_nonce;
    }
    std::string_view to() const override
    {
        return m_inner->data.to;
    }
    bcos::bytesConstRef input() const override
    {
        return bcos::bytesConstRef(
            reinterpret_cast<const bcos::byte*>(m_inner()->data.input.data()),
            m_inner()->data.input.size());
    }
    int64_t importTime() const override { return m_inner()->importTime; }
    void setImportTime(int64_t _importTime) override { m_inner()->importTime = _importTime; }
    bcos::bytesConstRef signatureData() const override
    {
        return bcos::bytesConstRef(reinterpret_cast<const bcos::byte*>(m_inner()->signature.data()),
            m_inner()->signature.size());
    }

    void setSignatureData(bcos::bytes& signature)
    {
        m_inner()->signature.assign(signature.begin(), signature.end());
    }

    const bcostars::Transaction& inner() const { return *m_inner(); }

    void setInner(const bcostars::Transaction& inner) { *m_inner() = inner; }

    void setInner(bcostars::Transaction&& inner) { *m_inner() = std::move(inner); }

private:
    std::function<bcostars::Transaction*()> m_inner;
    mutable bcos::bytes m_buffer;
    mutable bcos::bytes m_dataBuffer;
    mutable bcos::u256 m_nonce;
};

class TransactionFactoryImpl : public bcos::protocol::TransactionFactory
{
public:
    TransactionFactoryImpl(bcos::crypto::CryptoSuite::Ptr cryptoSuite) : m_cryptoSuite(cryptoSuite)
    {}
    ~TransactionFactoryImpl() override {}

    bcos::protocol::Transaction::Ptr createTransaction(
        bcos::bytesConstRef _txData, bool _checkSig = true) override
    {
        bcostars::Transaction tx;
        auto transaction = std::make_shared<TransactionImpl>(
            m_cryptoSuite, [m_transaction = std::move(tx)]() mutable { return &m_transaction; });

        transaction->decode(_txData);
        if (_checkSig)
        {
            transaction->verify();
        }
        return transaction;
    }

    bcos::protocol::Transaction::Ptr createTransaction(
        bcos::bytes const& _txData, bool _checkSig = true) override
    {
        return createTransaction(bcos::ref(_txData), _checkSig);
    }

    bcos::protocol::Transaction::Ptr createTransaction(int32_t _version,
        const std::string_view& _to, bcos::bytes const& _input, bcos::u256 const& _nonce,
        int64_t _blockLimit, std::string const& _chainId, std::string const& _groupId,
        int64_t _importTime) override
    {
        bcostars::Transaction tx;
        auto transaction = std::make_shared<bcostars::protocol::TransactionImpl>(
            m_cryptoSuite, [m_transaction = std::move(tx)]() mutable { return &m_transaction; });
        transaction->m_inner()->data.version = _version;
        transaction->m_inner()->data.to.assign(_to.begin(), _to.end());
        transaction->m_inner()->data.input.assign(_input.begin(), _input.end());
        transaction->m_inner()->data.blockLimit = _blockLimit;
        transaction->m_inner()->data.chainID = _chainId;
        transaction->m_inner()->data.groupID = _groupId;
        transaction->m_inner()->data.nonce = boost::lexical_cast<std::string>(_nonce);
        transaction->m_inner()->importTime = _importTime;

        return transaction;
    }

    bcos::protocol::Transaction::Ptr createTransaction(int32_t _version,
        const std::string_view& _to, bcos::bytes const& _input, bcos::u256 const& _nonce,
        int64_t _blockLimit, std::string const& _chainId, std::string const& _groupId,
        int64_t _importTime, bcos::crypto::KeyPairInterface::Ptr keyPair) override
    {
        auto tx = createTransaction(
            _version, _to, _input, _nonce, _blockLimit, _chainId, _groupId, _importTime);
        auto sign = m_cryptoSuite->signatureImpl()->sign(keyPair, tx->hash(), true);

        auto tarsTx = std::dynamic_pointer_cast<bcostars::protocol::TransactionImpl>(tx);
        tarsTx->m_inner()->signature.assign(sign->begin(), sign->end());

        return tx;
    }

    void setCryptoSuite(bcos::crypto::CryptoSuite::Ptr cryptoSuite) { m_cryptoSuite = cryptoSuite; }
    bcos::crypto::CryptoSuite::Ptr cryptoSuite() override { return m_cryptoSuite; }

private:
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};

}  // namespace protocol
}  // namespace bcostars