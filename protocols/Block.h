#pragma once

#include "tars/Block.h"
#include <bcos-framework/interfaces/protocol/Block.h>
#include <bcos-framework/interfaces/protocol/BlockFactory.h>

namespace bcos_tars {
namespace protocol {

class Block : public bcos::protocol::Block {
public:
  virtual ~Block() override;

  virtual void decode(bcos::bytesConstRef _data, bool _calculateHash,
                      bool _checkSig) override;
  virtual void encode(bcos::bytes &_encodeData) const override;
  virtual bcos::crypto::HashType
  calculateTransactionRoot(bool _updateHeader) const override;
  virtual bcos::crypto::HashType
  calculateReceiptRoot(bool _updateHeader) const override;

  virtual int32_t version() const override;
  virtual void setVersion(int32_t _version) override;
  // get blockHeader
  virtual bcos::protocol::BlockHeader::Ptr blockHeader() const override;
  // get transactions
  virtual bcos::protocol::TransactionsConstPtr transactions() override;
  virtual bcos::protocol::Transaction::ConstPtr
  transaction(size_t _index) override;
  // get receipts
  virtual bcos::protocol::ReceiptsConstPtr receipts() override;
  virtual bcos::protocol::TransactionReceipt::ConstPtr
  receipt(size_t _index) override;
  // get transaction hash
  virtual bcos::protocol::HashListConstPtr transactionsHash() override;
  virtual bcos::crypto::HashType const &transactionHash(size_t _index) override;
  // get receipt hash
  virtual bcos::protocol::HashListConstPtr receiptsHash() override;
  virtual bcos::crypto::HashType const &receiptHash(size_t _index) override;

  virtual void setBlockType(bcos::protocol::BlockType _blockType) override;
  // set blockHeader
  virtual void
  setBlockHeader(bcos::protocol::BlockHeader::Ptr _blockHeader) override;
  // set transactions
  virtual void
  setTransactions(bcos::protocol::TransactionsPtr _transactions) override;
  virtual void
  setTransaction(size_t _index,
                 bcos::protocol::Transaction::Ptr _transaction) override;
  virtual void
  appendTransaction(bcos::protocol::Transaction::Ptr _transaction) override;
  // set receipts
  virtual void setReceipts(bcos::protocol::ReceiptsPtr _receipts) override;
  virtual void
  setReceipt(size_t _index,
             bcos::protocol::TransactionReceipt::Ptr _receipt) override;
  virtual void
  appendReceipt(bcos::protocol::TransactionReceipt::Ptr _receipt) override;
  // set transaction hash
  virtual void
  setTransactionsHash(bcos::protocol::HashListPtr _transactionsHash) override;
  virtual void
  setTransactionHash(size_t _index,
                     bcos::crypto::HashType const &_txHash) override;
  virtual void
  appendTransactionHash(bcos::crypto::HashType const &_txHash) override;
  // set receipt hash
  virtual void
  setReceiptsHash(bcos::protocol::HashListPtr _receiptsHash) override;
  virtual void
  setReceiptHash(size_t _index,
                 bcos::crypto::HashType const &_receptHash) override;
  virtual void
  appendReceiptHash(bcos::crypto::HashType const &_receiptHash) override;
  // getNonces of the current block
  virtual bcos::protocol::NonceListPtr nonces() override;
  // get transactions size
  virtual size_t transactionsSize() override;
  virtual size_t transactionsHashSize() override;
  // get receipts size
  virtual size_t receiptsSize() override;
  virtual size_t receiptsHashSize() override;

private:
  bcos_tars::Block m_block;
};

class BlockFactory : public bcos::protocol::BlockFactory {
  virtual ~BlockFactory() {}
  virtual Block::Ptr createBlock() override;
  virtual Block::Ptr createBlock(bcos::bytes const &_data, bool _calculateHash = true,
                                 bool _checkSig = true) override;
};

} // namespace protocol
} // namespace bcos_tars