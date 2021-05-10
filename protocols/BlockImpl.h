#pragma once

#include "Block.h"
#include "Common.h"
#include <bcos-framework/interfaces/protocol/Block.h>
#include <bcos-framework/interfaces/protocol/BlockFactory.h>
#include <bcos-framework/interfaces/protocol/BlockHeader.h>
#include <bcos-framework/interfaces/protocol/BlockHeaderFactory.h>
#include <gsl/span>

namespace bcostars {
namespace protocol {

class Block : public bcos::protocol::Block {
public:
  virtual ~Block() override;

  virtual void decode(bcos::bytesConstRef _data, bool _calculateHash,
                      bool _checkSig) override {}
  virtual void encode(bcos::bytes &_encodeData) const override {}
  virtual bcos::crypto::HashType
  calculateTransactionRoot(bool _updateHeader) const override;
  virtual bcos::crypto::HashType
  calculateReceiptRoot(bool _updateHeader) const override;

  virtual int32_t version() const override;
  virtual void setVersion(int32_t _version) override;

  // get blockHeader
  virtual bcos::protocol::BlockHeader::Ptr blockHeader() override {
    return std::make_shared<BlockHeader>(&m_block.blockHeader);
  };

  virtual bcos::protocol::Transaction::ConstPtr
  transaction(size_t _index) const override {
    // return std::make_shared<Transaction>(&m_block.transactions[_index]);
  }

  virtual bcos::protocol::TransactionReceipt::ConstPtr
  receipt(size_t _index) const override {
    return (*m_transactionReceipts)[_index];
  };

  virtual bcos::crypto::HashType const &
  transactionHash(size_t _index) const override {
    return (*m_hashes)[_index];
  }

  virtual bcos::crypto::HashType const &
  receiptHash(size_t _index) const override {
    return (*m_receiptHashes)[_index];
  }

  virtual void setBlockType(bcos::protocol::BlockType _blockType) override {}
  // set blockHeader
  virtual void
  setBlockHeader(bcos::protocol::BlockHeader::Ptr _blockHeader) override {
    m_blockHeader = _blockHeader;
  }
  
  virtual void
  setTransaction(size_t _index,
                 bcos::protocol::Transaction::Ptr _transaction) override {
    (*m_transactions)[_index] = _transaction;
  }
  virtual void
  appendTransaction(bcos::protocol::Transaction::Ptr _transaction) override {
    m_transactions->push_back(_transaction);
  }

  virtual void
  setReceipt(size_t _index,
             bcos::protocol::TransactionReceipt::Ptr _receipt) override {
    (*m_transactionReceipts)[_index] = _receipt;
  }
  virtual void
  appendReceipt(bcos::protocol::TransactionReceipt::Ptr _receipt) override {
    m_transactionReceipts->push_back(_receipt);
  }

  virtual void
  setTransactionHash(size_t _index,
                     bcos::crypto::HashType const &_txHash) override {
    (*m_hashes)[_index] = _txHash;
  }
  virtual void
  appendTransactionHash(bcos::crypto::HashType const &_txHash) override {
    m_hashes->push_back(_txHash);
  }

  virtual void
  setReceiptHash(size_t _index,
                 bcos::crypto::HashType const &_receptHash) override {
    (*m_receiptHashes)[_index] = _receptHash;
  }
  virtual void
  appendReceiptHash(bcos::crypto::HashType const &_receiptHash) override {
    m_receiptHashes->push_back(_receiptHash);
  }
  // getNonces of the current block
  virtual bcos::protocol::NonceListPtr nonces() override { return m_nonce; }
  // get transactions size
  virtual size_t transactionsSize() override { return m_transactions->size(); }
  virtual size_t transactionsHashSize() override {
    return m_transactionReceipts->size();
  }
  // get receipts size
  virtual size_t receiptsSize() override { return m_receiptHashes->size(); }
  virtual size_t receiptsHashSize() override {
    return m_transactionReceipts->size();
  }

private:
  bcostars::Block m_inner;

  bcos::protocol::BlockHeader::Ptr m_blockHeader;
};

class BlockFactory : public bcos::protocol::BlockFactory {
  virtual ~BlockFactory() {}
  virtual Block::Ptr createBlock() override;
  virtual Block::Ptr createBlock(bcos::bytes const &_data,
                                 bool _calculateHash = true,
                                 bool _checkSig = true) override;
};

}
}