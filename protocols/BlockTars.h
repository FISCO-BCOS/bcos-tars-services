#pragma once

#include "Common.h"
#include "Block.h"
#include <bcos-framework/interfaces/protocol/Block.h>
#include <bcos-framework/interfaces/protocol/BlockFactory.h>
#include <bcos-framework/interfaces/protocol/BlockHeader.h>
#include <bcos-framework/interfaces/protocol/BlockHeaderFactory.h>

namespace bcostars {
namespace protocol {

class BlockHeader : public bcos::protocol::BlockHeader {
public:
  virtual ~BlockHeader() {}

  BlockHeader(bcostars::BlockHeader *blockHeader)
      : m_blockHeader(std::shared_ptr<bcostars::BlockHeader>(
            blockHeader, [](bcostars::BlockHeader *) {})){};

  virtual void decode(bcos::bytesConstRef _data) override {
    tars::TarsInputStream input;
    input.setBuffer((const char *)_data.data(), _data.size());

    m_blockHeader->readFrom(input);
  }

  virtual void encode(bcos::bytes &_encodeData) const override {
    tars::TarsOutputStream<bcostars::protocol::BufferWriterByteVector> output;

    m_blockHeader->writeTo(output);
    output.getByteBuffer().swap(_encodeData);
  }

  virtual bcos::crypto::HashType const &hash() const override {
    return m_hash;
  }
  virtual void
  populateFromParents(BlockHeadersPtr _parents,
                      bcos::protocol::BlockNumber _number) override {}
  virtual void clear() override {}
  // verify the signatureList
  virtual void verifySignatureList() const override {}
  virtual void populateEmptyBlock(int64_t _timestamp) override;

  // the version of the blockHeader
  virtual int32_t version() const override { return m_blockHeader->version; }
  // the parent information, including (parentBlockNumber, parentHash)
  virtual bcos::protocol::ParentInfoListPtr parentInfo() const override {}
  // the txsRoot of the current block
  virtual bcos::crypto::HashType const &txsRoot() const override {
    return bcos::crypto::HashType(m_blockHeader->txsRoot.data());
  }
  // the receiptRoot of the current block
  virtual bcos::crypto::HashType const &receiptRoot() const override;
  // the stateRoot of the current block
  virtual bcos::crypto::HashType const &stateRoot() const override;
  // the number of the current block
  virtual bcos::protocol::BlockNumber number() const override {
    return m_blockHeader->blockNumber;
  }
  virtual bcos::u256 const &gasUsed() override {
    return m_blockHeader->gasUsed;
  }
  virtual int64_t timestamp() override { return m_blockHeader->timestamp; }
  // the sealer that generate this block
  virtual int64_t sealer() override { return m_blockHeader->sealer; }
  // the current sealer list
  virtual bcos::protocol::BytesListPtr sealerList() const override {
    return bcos::protocol::BytesListPtr(&(m_blockHeader->sealerList));
  }
  virtual bcos::bytes const &extraData() const override {
    return m_blockHeader->extraData;
  }
  virtual bcos::protocol::SignatureListPtr signatureList() const override {
    return m_blockHeader->signatureList;
  }

  virtual bcos::protocol::WeightList const &consensusWeights() const override {
    return m_blockHeader->consensusWeights;
  }

  virtual void setVersion(int32_t _version) override {
    m_blockHeader->version = _version;
  }
  virtual void
  setParentInfo(bcos::protocol::ParentInfoListPtr _parentInfo) override {
    m_blockHeader->parentInfo.clear();
    for(auto& it: *_parentInfo) {
      ParentInfo parentInfo;
      parentInfo.blockNumber = it.first;
      parentInfo.hash.assign(it.second.begin(), it.second.end());
      m_blockHeader->parentInfo.emplace_back(parentInfo);
    }
  }
  virtual void setTxsRoot(bcos::crypto::HashType const &_txsRoot) override {
    m_blockHeader->txsRoot = _txsRoot;
  }
  virtual void
  setReceiptRoot(bcos::crypto::HashType const &_receiptRoot) override;
  virtual void setStateRoot(bcos::crypto::HashType const &_stateRoot) override;
  virtual void setNumber(bcos::protocol::BlockNumber _blockNumber) override;
  virtual void setGasUsed(bcos::u256 const &_gasUsed) override;
  virtual void setTimestamp(int64_t const &_timestamp) override;
  virtual void setSealer(int64_t _sealerId) override;
  virtual void
  setSealerList(bcos::protocol::BytesList const &_sealerList) override;

  virtual void
  setConsensusWeights(bcos::protocol::WeightListPtr _weightList) override;

  virtual void setExtraData(bcos::bytes const &_extraData) override;
  virtual void setExtraData(bcos::bytes &&_extraData) override;
  virtual void
  setSignatureList(bcos::protocol::SignatureListPtr _signatureList) override;

private:
  std::shared_ptr<bcostars::BlockHeader> m_blockHeader;
  bcos::crypto::HashType m_hash;
};

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
  // get receipts
  virtual bcos::protocol::ReceiptsConstPtr receipts() const override {
    return m_transactionReceipts;
  };
  virtual bcos::protocol::TransactionReceipt::ConstPtr
  receipt(size_t _index) const override {
    return (*m_transactionReceipts)[_index];
  };
  // get transaction hash
  virtual bcos::protocol::HashListConstPtr transactionsHash() const override {
    return m_hashes;
  }
  virtual bcos::crypto::HashType const &
  transactionHash(size_t _index) const override {
    return (*m_hashes)[_index];
  }
  // get receipt hash
  virtual bcos::protocol::HashListConstPtr receiptsHash() const override {
    return m_receiptHashes;
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
  // set transactions
  virtual void
  setTransactions(bcos::protocol::TransactionsPtr _transactions) override {
    m_transactions = _transactions;
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
  // set receipts
  virtual void setReceipts(bcos::protocol::ReceiptsPtr _receipts) override {
    m_transactionReceipts = _receipts;
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
  // set transaction hash
  virtual void
  setTransactionsHash(bcos::protocol::HashListPtr _transactionsHash) override {
    m_hashes = _transactionsHash;
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
  // set receipt hash
  virtual void
  setReceiptsHash(bcos::protocol::HashListPtr _receiptsHash) override {
    m_receiptHashes = _receiptsHash;
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
  bcostars::Block m_block;

  bcos::protocol::BlockHeader::Ptr m_blockHeader;

  bcos::protocol::TransactionsPtr m_transactions;
  bcos::protocol::ReceiptsPtr m_transactionReceipts;

  bcos::protocol::HashListPtr m_hashes;
  bcos::protocol::HashListPtr m_receiptHashes;

  bcos::protocol::NonceListPtr m_nonce;
};

class BlockFactory : public bcos::protocol::BlockFactory {
  virtual ~BlockFactory() {}
  virtual Block::Ptr createBlock() override;
  virtual Block::Ptr createBlock(bcos::bytes const &_data,
                                 bool _calculateHash = true,
                                 bool _checkSig = true) override;
};

} // namespace protocol
} // namespace bcostars