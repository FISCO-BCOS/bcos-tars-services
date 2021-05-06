#pragma once

#include "Block.h"
#include "Common.h"
#include <bcos-framework/interfaces/protocol/Block.h>
#include <bcos-framework/interfaces/protocol/BlockFactory.h>
#include <bcos-framework/interfaces/protocol/BlockHeader.h>
#include <bcos-framework/interfaces/protocol/BlockHeaderFactory.h>
#include <gsl/span>
#include <iterator>

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

  virtual bcos::crypto::HashType const& hash() const override {
    bcos::bytes encoded;
    encode(encoded);

    m_hash = m_cryptoSuite->hash(encoded);
    return m_hash;
  }
  void populateFromParents(BlockHeadersPtr _parents,
                           bcos::protocol::BlockNumber _number) override {
    (void)_parents;
    (void)_number;
  }
  void clear() override {}
  // verify the signatureList
  void verifySignatureList() const override {}
  void populateEmptyBlock(int64_t _timestamp) override;

  virtual int32_t version() const override { return m_blockHeader->version; }
  gsl::span<const bcos::protocol::ParentInfo> parentInfo() const override {}
  bcos::crypto::HashType const &txsRoot() const override {
    return *(reinterpret_cast<bcos::crypto::HashType *>(
        m_blockHeader->txsRoot.data()));
  }
  bcos::crypto::HashType const &receiptRoot() const override {
    return *(reinterpret_cast<bcos::crypto::HashType *>(
        m_blockHeader->receiptRoot.data()));
  }
  bcos::crypto::HashType const &stateRoot() const override {
    return *(reinterpret_cast<bcos::crypto::HashType *>(
        m_blockHeader->stateRoot.data()));
  }
  bcos::protocol::BlockNumber number() const override {
    return m_blockHeader->blockNumber;
  }
  bcos::u256 const &gasUsed() override {
    return *((bcos::u256 *)(m_blockHeader->gasUsed.data()));
  }
  int64_t timestamp() override { return m_blockHeader->timestamp; }
  int64_t sealer() override { return m_blockHeader->sealer; }
  gsl::span<const bcos::bytes> sealerList() const override {
    return m_blockHeader->sealerList;
  }
  bcos::bytesConstRef extraData() const override {
    return bcos::bytesConstRef(m_blockHeader->extraData.data(),
                               m_blockHeader->extraData.size());
  }
  gsl::span<const bcos::protocol::Signature> signatureList() const override {
    return gsl::span(reinterpret_cast<const bcos::protocol::Signature *>(
                         m_blockHeader->signatureList.data()),
                     m_blockHeader->signatureList.size());
  }

  virtual gsl::span<const uint64_t> consensusWeights() const override {
    return gsl::span(
        reinterpret_cast<uint64_t *>(m_blockHeader->consensusWeights.data()),
        m_blockHeader->consensusWeights.size());
  }

  void setVersion(int32_t _version) override {
    m_blockHeader->version = _version;
  }
  void setParentInfo(
      gsl::span<const bcos::protocol::ParentInfo> const &_parentInfo) override {
    m_blockHeader->parentInfo.clear();
    for (auto &it : _parentInfo) {
      ParentInfo parentInfo;
      parentInfo.blockNumber = it.blockNumber;
      *parentInfo.blockHash = *it.blockHash.data();
      m_blockHeader->parentInfo.emplace_back(parentInfo);
    }
  }
  virtual void setTxsRoot(bcos::crypto::HashType const &_txsRoot) override {
    m_blockHeader->txsRoot.assign(_txsRoot.begin(), _txsRoot.end());
  }

  virtual void
  setReceiptRoot(bcos::crypto::HashType const &_receiptRoot) override {
    m_blockHeader->receiptRoot.assign(_receiptRoot.begin(), _receiptRoot.end());
  }
  virtual void setStateRoot(bcos::crypto::HashType const &_stateRoot) override {
    m_blockHeader->stateRoot.assign(_stateRoot.begin(), _stateRoot.end());
  }
  virtual void setNumber(bcos::protocol::BlockNumber _blockNumber) override {
    m_blockHeader->blockNumber = _blockNumber;
  }
  virtual void setGasUsed(bcos::u256 const &_gasUsed) override {}
  virtual void setTimestamp(int64_t _timestamp) override {
    m_blockHeader->timestamp = _timestamp;
  }
  virtual void setSealer(int64_t _sealerId) override {
    m_blockHeader->sealer = _sealerId;
  }
  virtual void
  setSealerList(gsl::span<const bcos::bytes> const &_sealerList) override {
    m_blockHeader->sealerList.assign(_sealerList.begin(), _sealerList.end());
  }

  virtual void
  setConsensusWeights(gsl::span<const uint64_t> const &_weightList) override {
    m_blockHeader->consensusWeights.assign(_weightList.begin(),
                                           _weightList.end());
  }

  virtual void setExtraData(bcos::bytes const &_extraData) override {
    m_blockHeader->extraData = _extraData;
  }
  virtual void setExtraData(bcos::bytes &&_extraData) override {
    m_blockHeader->extraData = std::move(_extraData);
  }
  virtual void setSignatureList(gsl::span<const bcos::protocol::Signature> const
                                    &_signatureList) override {
    for (auto &it : _signatureList) {
      Signature signature;
      signature.sealerIndex = it.index;
      signature.signature = it.signature;
      m_blockHeader->signatureList.emplace_back(signature);
    }
  }

private:
  std::shared_ptr<bcostars::BlockHeader> m_blockHeader;
  mutable bcos::crypto::HashType m_hash;
  bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
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
  bcostars::Block m_block;

  bcos::protocol::BlockHeader::Ptr m_blockHeader;
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