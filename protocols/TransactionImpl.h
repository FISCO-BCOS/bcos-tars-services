#pragma once

#include "Common.h"
#include "Transaction.h"
#include "bcos-framework/libutilities/DataConvertUtility.h"
#include <bcos-framework/interfaces/crypto/CommonType.h>
#include <bcos-framework/interfaces/protocol/Transaction.h>
#include <bcos-framework/interfaces/protocol/TransactionFactory.h>
#include <bcos-framework/libutilities/Common.h>

namespace bcostars {
namespace protocol {
class Transaction : public bcos::protocol::Transaction {
public:
  ~Transaction() {}

  bool operator==(const Transaction &rhs) const {
    return this->hash() == rhs.hash();
  }

  void decode(bcos::bytesConstRef _txData, bool _checkSig) override {
    tars::TarsInputStream input;
    input.setBuffer((const char *)_txData.data(), _txData.size());
    (void)_checkSig;

    m_inner->readFrom(input);
    m_nonce = boost::lexical_cast<bcos::u256>(m_inner->nonce);
  }

  void encode(bcos::bytes &_txData) const override {
    tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;

    m_inner->nonce = boost::lexical_cast<std::string>(m_nonce);
    m_inner->writeTo(output);
    output.getByteBuffer().swap(_txData);
  }

  bcos::crypto::HashType const &hash() const override {
    bcos::bytes encoded;

    encode(encoded);
    m_hash = m_cryptoSuite->hash(encoded);

    return m_hash;
  }

  int32_t version() const override { return m_inner->version; }
  std::string_view chainId() const override { return m_inner->chainID; }
  std::string_view groupId() const override { return m_inner->groupID; }
  int64_t blockLimit() const override { return m_inner->blockLimit; }
  bcos::u256 const &nonce() const override {
    return m_nonce;
  }
  bcos::bytesConstRef to() const override {
    return bcos::bytesConstRef((const unsigned char *)m_inner->to.data(),
                               m_inner->to.size());
  }
  bcos::bytesConstRef sender() const override {
    return bcos::bytesConstRef(
        (const unsigned char *)m_inner->sender.data(),
        m_inner->sender.size());
  }
  bcos::bytesConstRef input() const override {
    return bcos::bytesConstRef(
        (const unsigned char *)m_inner->input.data(),
        m_inner->input.size());
  }
  virtual int64_t importTime() const override {
    return m_inner->importTime;
  }
  virtual bcos::protocol::TransactionType type() const override {
    return (bcos::protocol::TransactionType)m_inner->type;
  }
  virtual void forceSender(bcos::bytes const &_sender) override {
    m_inner->sender.assign(_sender.begin(), _sender.end());
  }

  void setNonce(bcos::u256 nonce) {
    m_nonce = nonce;
  }

  void setInner(std::shared_ptr<bcostars::Transaction> transaction) {
    m_inner = transaction;
  }

  void setCryptoSuite(bcos::crypto::CryptoSuite::Ptr cryptoSuite) {
    m_cryptoSuite = cryptoSuite;
  }

private:
  mutable bcos::crypto::HashType m_hash;

  std::shared_ptr<bcostars::Transaction> m_inner;
  bcos::u256 m_nonce;

  bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};

class TransactionFactory : public bcos::protocol::TransactionFactory {
public:
  ~TransactionFactory() override {}

  Transaction::Ptr createTransaction(bcos::bytesConstRef _txData,
                                     bool _checkSig = true) override {
    auto inner = std::make_shared<bcostars::Transaction>();
    auto transaction = std::make_shared<Transaction>();
    transaction->setInner(inner);
    transaction->setCryptoSuite(m_cryptoSuite);

    transaction->decode(_txData, _checkSig);
    return transaction;
  }

  Transaction::Ptr createTransaction(bcos::bytes const &_txData,
                                     bool _checkSig = true) override {
    return createTransaction(bcos::ref(_txData), _checkSig);
  }

  Transaction::Ptr
  createTransaction(int32_t const &_version, bcos::bytes const &_to,
                    bcos::bytes const &_input, bcos::u256 const &_nonce,
                    int64_t const &_blockLimit, std::string const &_chainId,
                    std::string const &_groupId,
                    int64_t const &_importTime) override {
    auto inner = std::make_shared<bcostars::Transaction>();
    inner->version = _version;
    inner->to = _to;
    inner->input = _input;
    inner->blockLimit = _blockLimit;
    inner->chainID = _chainId;
    inner->groupID = _groupId;
    inner->importTime = _importTime;

    auto transaction = std::make_shared<bcostars::protocol::Transaction>();
    transaction->setInner(inner);
    transaction->setNonce(_nonce);
    transaction->setCryptoSuite(m_cryptoSuite);

    return transaction;
  }

  bcos::crypto::CryptoSuite::Ptr cryptoSuite() override {
    return m_cryptoSuite;
  }

  void setCryptoSuite(bcos::crypto::CryptoSuite::Ptr cryptoSuite) {
    m_cryptoSuite = cryptoSuite;
  }

  bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};

} // namespace protocol
} // namespace bcostars