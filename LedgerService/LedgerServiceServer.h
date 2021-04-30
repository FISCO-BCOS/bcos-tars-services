#pragma once

#include "LedgerService.h"
#include "servant/Application.h"
#include <bcos-framework/interfaces/ledger/LedgerInterface.h>
#include "../fake/FakeLedger.h"
#include "../Clients/StorageServiceClient.h"

namespace bcostars {

class LedgerServiceServer : public LedgerService {
public:
  virtual ~LedgerServiceServer() override {}

  virtual void initialize() override;

  virtual void destroy() override;

  virtual tars::Int32 commitBlock(tars::Int64 blockNumber,
                                  const vector<vector<tars::Char>> &signList,
                                  tars::TarsCurrentPtr current) override;
  virtual tars::Int32 getBlockByHash(const vector<tars::Char> &blockHash,
                                     bcostars::Block &block,
                                     tars::TarsCurrentPtr current) override;
  virtual tars::Int32 getBlockByNumber(tars::Int64 blockNumber,
                                       bcostars::Block &block,
                                       tars::TarsCurrentPtr current) override;
  virtual tars::Int32
  getBlockHashByNumber(tars::Int64 blockNumber, vector<tars::Char> &blockHash,
                       tars::TarsCurrentPtr current) override;
  virtual tars::Int32
  getBlockHeaderByHash(const vector<tars::Char> &blockHash,
                       bcostars::BlockHeader &blockHeader,
                       vector<vector<tars::Char>> &sigList,
                       tars::TarsCurrentPtr current) override;
  virtual tars::Int32
  getBlockHeaderByNumber(tars::Int64 blockNumber,
                         bcostars::BlockHeader &blockHeader,
                         vector<vector<tars::Char>> &sigList,
                         tars::TarsCurrentPtr current) override;
  virtual tars::Int32 getBlockNumber(tars::Int64 &blockNumber,
                                     tars::TarsCurrentPtr current) override;
  virtual tars::Int32 getCode(const std::string &tableID,
                              const vector<tars::Char> &codeAddress,
                              vector<tars::Char> &code,
                              tars::TarsCurrentPtr current) override;
  virtual tars::Int32 getNonceList(tars::Int64 blockNumber,
                                   const vector<tars::Char> &nonceList,
                                   tars::TarsCurrentPtr current) override;
  virtual tars::Int32
  getTotalTransactionCount(tars::Int64 &totalTx, tars::Int64 &failedTx,
                           tars::Int64 &blockNumber,
                           tars::TarsCurrentPtr current) override;
  virtual tars::Int32 getTransactionByBlockHashAndIndex(
      const vector<tars::Char> &blockHash, tars::Int64 index,
      bcostars::Transaction &tx, tars::TarsCurrentPtr current) override;
  virtual tars::Int32 getTransactionByBlockNumberAndIndex(
      tars::Int64 blockNumber, tars::Int64 index, bcostars::Transaction &tx,
      tars::TarsCurrentPtr current) override;
  virtual tars::Int32
  getTransactionProof(const vector<tars::Char> &blockHash, tars::Int64 index,
                      vector<std::string> &proof,
                      tars::TarsCurrentPtr current) override;
  virtual tars::Int32
  getTransactionReceiptByHash(const vector<tars::Char> &hash,
                              bcostars::Transaction &tx,
                              tars::TarsCurrentPtr current) override;
  virtual tars::Int32
  getTransactionReceiptProof(const vector<tars::Char> &block, tars::Int64 index,
                             vector<std::string> &proof,
                             tars::TarsCurrentPtr current) override;
  virtual tars::Int32 getTxByHash(const vector<tars::Char> &hash,
                                  bcostars::Transaction &tx,
                                  tars::TarsCurrentPtr current) override;
  virtual tars::Int32 preStoreTxs(const vector<bcostars::Block> &blocks,
                                  tars::TarsCurrentPtr current) override;
private:
  std::shared_ptr<bcos::ledger::LedgerInterface> m_ledger;
  bcostars::StorageServiceClient::Ptr m_storageServiceClient;
};

} // namespace bcostars
/////////////////////////////////////////////////////
