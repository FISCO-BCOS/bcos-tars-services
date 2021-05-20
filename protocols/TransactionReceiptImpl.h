#pragma once

#include "Common.h"
#include "TransactionReceipt.h"
#include "bcos-framework/libutilities/Common.h"
#include "bcos-framework/libutilities/FixedBytes.h"
#include <bcos-framework/interfaces/crypto/CryptoSuite.h>
#include <bcos-framework/interfaces/crypto/Hash.h>
#include <bcos-framework/interfaces/protocol/TransactionReceipt.h>
#include <bcos-framework/interfaces/protocol/TransactionReceiptFactory.h>
#include <bcos-framework/libprotocol/LogEntry.h>

namespace bcostars {
namespace protocol {
class TransactionReceipt : public bcos::protocol::TransactionReceipt {
public:
  TransactionReceipt(bcos::crypto::CryptoSuite::Ptr _cryptoSuite)
      : bcos::protocol::TransactionReceipt(_cryptoSuite) {}
  ~TransactionReceipt() override {}

  friend class TransactionReceiptFactory;

  void decode(bcos::bytesConstRef _receiptData) override {
    m_buffer.assign(_receiptData.begin(), _receiptData.end());

    tars::TarsInputStream input;
    input.setBuffer((const char *)m_buffer.data(), m_buffer.size());

    m_inner.readFrom(input);
    m_gasUsed = boost::lexical_cast<bcos::u256>(m_inner.gasUsed);
    for (auto &it : m_inner.logEntries) {
      std::vector<bcos::h256> topics;
      for (auto &topicIt : it.topic) {
        topics.emplace_back(topicIt.data(), topicIt.size());
      }
      bcos::protocol::LogEntry logEntry(it.address, topics, it.data);
      m_logEntries.emplace_back(logEntry);
    }
  };

  void encode(bcos::bytes &_encodedData) const override {
    tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;

    m_inner.gasUsed = boost::lexical_cast<std::string>(m_gasUsed);
    m_inner.logEntries.clear();
    for (auto &it : m_logEntries) {
      bcostars::LogEntry logEntry;
      logEntry.address.assign(it.address().begin(), it.address().end());
      for (auto &topicIt : it.topics()) {
        logEntry.topic.push_back(
            std::vector<unsigned char>(topicIt.begin(), topicIt.end()));
      }
      logEntry.data.assign(it.data().begin(), it.data().end());

      m_inner.logEntries.emplace_back(logEntry);
    }

    m_inner.writeTo(output);
    output.getByteBuffer().swap(_encodedData);
  }

  bcos::bytesConstRef encode(bool _onlyHashFieldData = false) const override {
    if (m_buffer.empty()) {
      encode(m_buffer);
    }

    return bcos::ref(m_buffer);
  }

  int32_t version() const override { return m_inner.version; }
  bcos::crypto::HashType const &stateRoot() const override {
    return *(
        reinterpret_cast<bcos::crypto::HashType *>(m_inner.stateRoot.data()));
  }
  bcos::u256 const &gasUsed() const override { return m_gasUsed; }

  bcos::bytesConstRef contractAddress() const override {
    return bcos::bytesConstRef(
        (const unsigned char *)m_inner.contractAddress.data(),
        m_inner.contractAddress.size());
  }
  bcos::LogBloom const &bloom() const override {
    return *(reinterpret_cast<bcos::LogBloom *>(m_inner.bloom));
  }
  int32_t status() const override { return m_inner.status; }
  bcos::bytesConstRef output() const override {
    return bcos::bytesConstRef((const unsigned char *)m_inner.output.data(),
                               m_inner.output.size());
  }
  gsl::span<const bcos::protocol::LogEntry> logEntries() const override {
    return gsl::span<const bcos::protocol::LogEntry>(m_logEntries.data(),
                                                     m_logEntries.size());
  }
  bcos::protocol::BlockNumber blockNumber() const override {
    return m_inner.blockNumber;
  }

private:
  mutable bcos::crypto::HashType m_hash;

  mutable bcos::bytes m_buffer;
  mutable bcostars::TransactionReceipt m_inner;
  bcos::u256 m_gasUsed;
  std::vector<bcos::protocol::LogEntry> m_logEntries;
};

class TransactionReceiptFactory
    : public bcos::protocol::TransactionReceiptFactory {
public:
  ~TransactionReceiptFactory() override {}

  TransactionReceipt::Ptr
  createReceipt(bcos::bytesConstRef _receiptData) override {
    auto transactionReceipt = std::make_shared<TransactionReceipt>(m_cryptoSuite);

    transactionReceipt->decode(_receiptData);

    return transactionReceipt;
  }

  TransactionReceipt::Ptr
  createReceipt(bcos::bytes const &_receiptData) override {
    return createReceipt(bcos::ref(_receiptData));
  }

  TransactionReceipt::Ptr createReceipt(
      int32_t _version, bcos::crypto::HashType const &_stateRoot,
      bcos::u256 const &_gasUsed, bcos::bytes const &_contractAddress,
      std::shared_ptr<std::vector<bcos::protocol::LogEntry>> _logEntries,
      int32_t _status, bcos::bytes const &_output,
      bcos::protocol::BlockNumber _blockNumber) override {
    auto transactionReceipt =
        std::make_shared<TransactionReceipt>(m_cryptoSuite);
    transactionReceipt->m_inner.version = _version;
    transactionReceipt->m_inner.stateRoot.assign(_stateRoot.begin(),
                                                 _stateRoot.end());
    transactionReceipt->m_inner.contractAddress = _contractAddress;
    transactionReceipt->m_inner.status = _status;
    transactionReceipt->m_inner.output = _output;

    transactionReceipt->m_gasUsed = _gasUsed;
    _logEntries->swap(transactionReceipt->m_logEntries);
    transactionReceipt->m_inner.blockNumber = _blockNumber;

    return transactionReceipt;
  }

  TransactionReceipt::Ptr createReceipt(
      int32_t _version, bcos::crypto::HashType const &_stateRoot,
      bcos::u256 const &_gasUsed, bcos::bytes const &_contractAddress,
      std::shared_ptr<std::vector<bcos::protocol::LogEntry>> _logEntries,
      int32_t _status, bcos::bytes &&_output,
      bcos::protocol::BlockNumber _blockNumber) override {
    return createReceipt(_version, _stateRoot, _gasUsed, _contractAddress,
                         _logEntries, _status, _output, _blockNumber);
  }

  void setCryptoSuite(bcos::crypto::CryptoSuite::Ptr cryptoSuite) {
    m_cryptoSuite = cryptoSuite;
  }
  bcos::crypto::CryptoSuite::Ptr cryptoSuite() override {
    return m_cryptoSuite;
  }

  bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
} // namespace protocol
} // namespace bcostars