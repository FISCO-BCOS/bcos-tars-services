#pragma once

#include "../Clients/StorageServiceClient.h"
#include "StorageService.h"
#include "bcos-framework/libutilities/Error.h"
#include <bcos-framework/interfaces/ledger/LedgerInterface.h>

namespace bcostars {
class FakeLedger : public bcos::ledger::LedgerInterface {
public:
  ~FakeLedger() override {}

  void asyncCommitBlock(
      bcos::protocol::BlockNumber _blockNumber,
      const gsl::span<const bcos::protocol::Signature> &_signList,
      std::function<void(bcos::Error::Ptr)> _onCommitBlock) override{};

  void asyncPreStoreTransactions(
      bcos::protocol::Blocks const &_txsToStore,
      std::function<void(bcos::Error::Ptr)> _onTxsStored) override{};

  void asyncGetTransactionsByBlockNumber(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<void(bcos::Error::Ptr,
                         bcos::protocol::TransactionsConstPtr)>
          _onGetTx) override {
    auto tableInfo =
        std::make_shared<bcos::storage::TableInfo>("sys_transactions");
    auto condition = std::make_shared<bcos::storage::Condition>();
    auto result = m_storageServiceClient->getPrimaryKeys(tableInfo, condition);

    bcos::protocol::TransactionsConstPtr tx;
    _onGetTx(std::make_shared<bcos::Error>(0, result[0]), tx);
  };

  void asyncGetTransactionByHash(
      bcos::crypto::HashType const &_txHash,
      std::function<void(bcos::Error::Ptr,
                         bcos::protocol::Transaction::ConstPtr)>
          _onGetTx) override{};

  void asyncGetTransactionByBlockHashAndIndex(
      bcos::crypto::HashType const &_blockHash, int64_t _index,
      std::function<void(bcos::Error::Ptr,
                         bcos::protocol::Transaction::ConstPtr)>
          _onGetTx) override{};

  void asyncGetTransactionByBlockNumberAndIndex(
      bcos::protocol::BlockNumber _blockNumber, int64_t _index,
      std::function<void(bcos::Error::Ptr,
                         bcos::protocol::Transaction::ConstPtr)>
          _onGetTx) override{};

  void asyncGetTransactionReceiptByHash(
      bcos::crypto::HashType const &_txHash,
      std::function<void(bcos::Error::Ptr,
                         bcos::protocol::TransactionReceipt::ConstPtr)>
          _onGetTx) override{};

  void asyncGetReceiptsByBlockNumber(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<void(bcos::Error::Ptr, bcos::protocol::ReceiptsConstPtr)>
          _onGetReceipt) override{};

  void asyncGetTotalTransactionCount(
      std::function<void(bcos::Error::Ptr, int64_t _totalTxCount,
                         int64_t _failedTxCount,
                         bcos::protocol::BlockNumber _latestBlockNumber)>
          _callback) override{};

  void asyncGetTransactionReceiptProof(
      bcos::protocol::BlockNumber _blockNumber, int64_t _index,
      std::function<void(bcos::Error::Ptr, MerkleProofPtr)> _onGetProof)
      override{};

  void asyncGetTransactionProof(
      bcos::protocol::BlockNumber _blockNumber, int64_t _index,
      std::function<void(bcos::Error::Ptr, MerkleProofPtr)> _onGetProof)
      override{};

  void asyncGetTransactionProofByHash(
      bcos::crypto::HashType const &_txHash,
      std::function<void(bcos::Error::Ptr, MerkleProofPtr)> _onGetProof)
      override{};

  virtual void asyncGetTransactionReceiptProofByHash(
      bcos::crypto::HashType const &_txHash,
      std::function<void(bcos::Error::Ptr, MerkleProofPtr)> _onGetProof)
      override {}

  void asyncGetBlockNumber(
      std::function<void(bcos::Error::Ptr, bcos::protocol::BlockNumber)>
          _onGetBlock) override{};

  void asyncGetBlockHashByNumber(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<void(bcos::Error::Ptr, const bcos::crypto::HashType)>
          _onGetBlock) override{};
  void asyncGetBlockNumberByHash(
      const bcos::crypto::HashType &_blockHash,
      std::function<void(bcos::Error::Ptr, bcos::protocol::BlockNumber)>
          _onGetBlock) override{};

  void asyncGetBlockByHash(
      bcos::crypto::HashType const &_blockHash,
      std::function<void(bcos::Error::Ptr, bcos::protocol::Block::Ptr)>
          _onGetBlock) override{};

  void asyncGetBlockByNumber(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<void(bcos::Error::Ptr, bcos::protocol::Block::Ptr)>
          _onGetBlock) override{};

  void asyncGetBlockEncodedByNumber(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<void(bcos::Error::Ptr, bcos::bytesPointer)> _onGetBlock)
      override{};
  void asyncGetBlockHeaderByNumber(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<void(bcos::Error::Ptr, bcos::protocol::BlockHeader::Ptr)>
          _onGetBlock) override{};

  void asyncGetBlockHeaderByHash(
      bcos::crypto::HashType const &_blockHash,
      std::function<void(bcos::Error::Ptr, bcos::protocol::BlockHeader::Ptr)>
          _onGetBlock) override{};

  void
  asyncGetSystemConfigByKey(std::string const &_key,
                            std::function<void(bcos::Error::Ptr, std::string,
                                               bcos::protocol::BlockNumber)>
                                _onGetConfig) override{};

  void asyncGetNonceList(
      bcos::protocol::BlockNumber _blockNumber,
      std::function<void(bcos::Error::Ptr, bcos::protocol::NonceListPtr)>
          _onGetList) override{};

  void setStorageServiceClient(
      bcostars::StorageServiceClient::Ptr storageServiceClient) {
    m_storageServiceClient = storageServiceClient;
  }

private:
  bcos::storage::StorageInterface::Ptr m_storageServiceClient;
};
} // namespace bcostars