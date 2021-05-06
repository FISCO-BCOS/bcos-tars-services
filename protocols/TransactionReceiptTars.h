#pragma once

#include "Common.h"
#include "TransactionReceipt.h"
#include "bcos-framework/libutilities/Common.h"
#include "bcos-framework/libutilities/FixedBytes.h"
#include <bcos-framework/interfaces/crypto/CryptoSuite.h>
#include <bcos-framework/interfaces/crypto/Hash.h>
#include <bcos-framework/interfaces/protocol/TransactionReceipt.h>

namespace bcostars {
namespace protocol {
class TransactionReceipt : public bcos::protocol::TransactionReceipt {
public:
  ~TransactionReceipt() override {}

  void decode(bcos::bytesConstRef _receiptData) override {
    tars::TarsInputStream input;
    input.setBuffer((const char *)_receiptData.data(), _receiptData.size());

    m_transactionReceipt->readFrom(input);
  };
  void encode(bcos::bytes &_encodedData) override {
    tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;

    m_transactionReceipt->writeTo(output);
    output.getByteBuffer().swap(_encodedData);
  }
  bcos::crypto::HashType const &hash() override {
    bcos::bytes encoded;

    encode(encoded);
    m_hash = m_cryptoSuite->hash(encoded);

    return m_hash;
  }

  int32_t version() const override { return m_transactionReceipt->version; }
  bcos::crypto::HashType const &stateRoot() const override {
    return *(reinterpret_cast<bcos::crypto::HashType *>(
        m_transactionReceipt->stateRoot.data()));
  }
  bcos::u256 const &gasUsed() const override { return m_gasUsed; }

  bcos::bytesConstRef contractAddress() const override {
    return bcos::bytesConstRef(
        (const unsigned char *)m_transactionReceipt->contractAddress.data(),
        m_transactionReceipt->contractAddress.size());
  }
  bcos::LogBloom const &bloom() const override {
      return *(reinterpret_cast<bcos::LogBloom *>(
        m_transactionReceipt->bloom));
  }
  int32_t status() const override {
      return m_transactionReceipt->status;
  }
  bcos::bytesConstRef output() const override {
      return bcos::bytesConstRef((const unsigned char *)m_transactionReceipt->output.data(), m_transactionReceipt->output.size());
  }
  gsl::span<const bcos::protocol::LogEntry> logEntries() const override {
      return gsl::span<const bcos::protocol::LogEntry>(m_logEntries.data(), m_logEntries.size());
  }

private:
  mutable bcos::crypto::HashType m_hash;

  std::shared_ptr<bcostars::TransactionReceipt> m_transactionReceipt;
  bcos::u256 m_gasUsed;
  std::vector<bcos::protocol::LogEntry> m_logEntries;

  bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
} // namespace protocol
} // namespace bcostars