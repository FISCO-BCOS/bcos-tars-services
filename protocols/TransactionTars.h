#pragma once

#include "Common.h"
#include "Transaction.h"
#include "bcos-framework/libutilities/Common.h"
#include <bcos-framework/interfaces/protocol/Transaction.h>

namespace bcostars {
namespace protocol {
class Transaction : public bcos::protocol::Transaction {
public:
  virtual ~Transaction() {}

  virtual void decode(bcos::bytesConstRef _txData, bool _checkSig) override {
      tars::TarsInputStream input;
      input.setBuffer((const char*)_txData.data(), _txData.size());

      m_transaction->readFrom(input);
  }
  virtual void encode(bcos::bytes &_txData) const override {
      tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;

      m_transaction->writeTo(output);
      output.getByteBuffer().swap(_txData);
      _txData.swap(output.getByteBuffer());
  }
  virtual bcos::crypto::HashType const &hash() const override {

  }

  virtual int32_t version() const override {
      return m_transaction->version;
  }
  virtual std::string_view chainId() const override {
      return m_transaction->chainID;
  }
  virtual std::string_view groupId() const override {
      return m_transaction->groupID;
  }
  virtual int64_t blockLimit() const override {
      return m_transaction->blockLimit;
  }
  virtual bcos::u256 nonce() const override {
      return bcos::u256(m_transaction->nonce);
  }
  virtual bcos::bytesConstRef to() const override {
      return bcos::bytesConstRef((const unsigned char*)m_transaction->to.data(), m_transaction->to.size());
  }
  virtual bcos::bytesConstRef sender() const override {
      return bcos::bytesConstRef((const unsigned char*)m_transaction->sender.data(), m_transaction->sender.size());
  }
  virtual bcos::bytesConstRef input() const override {
      return bcos::bytesConstRef((const unsigned char*)m_transaction->input.data(), m_transaction->input.size());
  }
  virtual int64_t importTime() const override {
      return m_transaction->importTime;
  }
  virtual bcos::protocol::TransactionType type() const override {
      return (bcos::protocol::TransactionType)m_transaction->type;
  }
  virtual void forceSender(bcos::bytes const &_sender) override {
      m_transaction->sender.assign(_sender.begin(), _sender.end());
  }

  private:
  bcostars::Transaction *m_transaction;
}
}; // namespace protocol
} // namespace bcostars