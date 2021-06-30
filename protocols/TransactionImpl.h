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
class TransactionImpl : public bcos::protocol::Transaction {
public:
  explicit TransactionImpl(bcos::crypto::CryptoSuite::Ptr _cryptoSuite)
      : bcos::protocol::Transaction(_cryptoSuite) {
    m_inner = std::make_shared<bcostars::Transaction>();
  }

  explicit TransactionImpl(const bcostars::Transaction *transaction,
                           bcos::crypto::CryptoSuite::Ptr _cryptoSuite)
      : bcos::protocol::Transaction(_cryptoSuite) {
    m_inner = std::shared_ptr<bcostars::Transaction>(
        (bcostars::Transaction *)transaction, [](bcostars::Transaction *) {});
  }

  ~TransactionImpl() {}

  friend class TransactionFactoryImpl;

  bool operator==(const Transaction &rhs) const {
    return this->hash() == rhs.hash();
  }

  void decode(bcos::bytesConstRef _txData) override {
    m_buffer.assign(_txData.begin(), _txData.end());

    tars::TarsInputStream<tars::BufferReader> input;
    input.setBuffer((const char *)m_buffer.data(), m_buffer.size());

    m_inner->readFrom(input);
  }

  bcos::bytesConstRef encode(bool _onlyHashFields = false) const override {
    if (_onlyHashFields) {
      if (m_dataBuffer.empty()) {
        tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector>
            output;
        m_inner->data.writeTo(output);
        output.getByteBuffer().swap(m_dataBuffer);
      }

      return bcos::ref(m_dataBuffer);
    } else {
      if (m_buffer.empty()) {
        tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector>
            output;

        auto hash = m_cryptoSuite->hash(m_dataBuffer);
        m_inner->dataHash.assign(hash.begin(), hash.end());
        m_inner->writeTo(output);
        output.getByteBuffer().swap(m_buffer);
      }
      return bcos::ref(m_buffer);
    }
  }

  bcos::crypto::HashType const &hash() const override {
    if (m_inner->dataHash.empty()) {
      auto buffer = encode(true);
      auto hash = m_cryptoSuite->hash(buffer);
      m_inner->dataHash.assign(hash.begin(), hash.end());
    }

    return *(
        reinterpret_cast<bcos::crypto::HashType *>(m_inner->dataHash.data()));
  }

  int32_t version() const override { return m_inner->data.version; }
  std::string_view chainId() const override { return m_inner->data.chainID; }
  std::string_view groupId() const override { return m_inner->data.groupID; }
  int64_t blockLimit() const override { return m_inner->data.blockLimit; }
  bcos::u256 const &nonce() const override {
    m_nonce = boost::lexical_cast<bcos::u256>(m_inner->data.nonce);
    return m_nonce;
  }
  bcos::bytesConstRef to() const override {
    return bcos::bytesConstRef(m_inner->data.to.data(),
                               m_inner->data.to.size());
  }
  bcos::bytesConstRef input() const override {
    return bcos::bytesConstRef(m_inner->data.input.data(),
                               m_inner->data.input.size());
  }
  int64_t importTime() const override { return m_inner->importTime; }
  void setImportTime(int64_t _importTime) override {
    m_inner->importTime = _importTime;
  }
  bcos::bytesConstRef signatureData() const override {
    return bcos::ref(m_inner->signature);
  }

  void setSignatureData(bcos::bytes &signature) {
    signature.swap(m_inner->signature);
  }

  const bcostars::Transaction &inner() const { return *m_inner; }

  void setInner(const bcostars::Transaction &inner) { *m_inner = inner; }

private:
  mutable std::shared_ptr<bcostars::Transaction> m_inner;
  mutable bcos::bytes m_buffer;
  mutable bcos::bytes m_dataBuffer;
  mutable bcos::u256 m_nonce;
};

class TransactionFactoryImpl : public bcos::protocol::TransactionFactory {
public:
  TransactionFactoryImpl(bcos::crypto::CryptoSuite::Ptr cryptoSuite)
      : m_cryptoSuite(cryptoSuite) {}
  ~TransactionFactoryImpl() override {}

  bcos::protocol::Transaction::Ptr
  createTransaction(bcos::bytesConstRef _txData,
                    bool _checkSig = true) override {
    auto transaction = std::make_shared<TransactionImpl>(m_cryptoSuite);

    transaction->decode(_txData);
    if (_checkSig) {
      transaction->verify();
    }
    return transaction;
  }

  bcos::protocol::Transaction::Ptr
  createTransaction(bcos::bytes const &_txData,
                    bool _checkSig = true) override {
    return createTransaction(bcos::ref(_txData), _checkSig);
  }

  bcos::protocol::Transaction::Ptr
  createTransaction(int32_t const &_version, bcos::bytes const &_to,
                    bcos::bytes const &_input, bcos::u256 const &_nonce,
                    int64_t const &_blockLimit, std::string const &_chainId,
                    std::string const &_groupId,
                    int64_t const &_importTime) override {
    auto transaction =
        std::make_shared<bcostars::protocol::TransactionImpl>(m_cryptoSuite);
    transaction->m_inner->data.version = _version;
    transaction->m_inner->data.to = _to;
    transaction->m_inner->data.input = _input;
    transaction->m_inner->data.blockLimit = _blockLimit;
    transaction->m_inner->data.chainID = _chainId;
    transaction->m_inner->data.groupID = _groupId;
    transaction->m_inner->importTime = _importTime;

    transaction->m_inner->data.nonce = boost::lexical_cast<std::string>(_nonce);

    return transaction;
  }

  void setCryptoSuite(bcos::crypto::CryptoSuite::Ptr cryptoSuite) {
    m_cryptoSuite = cryptoSuite;
  }
  bcos::crypto::CryptoSuite::Ptr cryptoSuite() override {
    return m_cryptoSuite;
  }

private:
  bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};

} // namespace protocol
} // namespace bcostars