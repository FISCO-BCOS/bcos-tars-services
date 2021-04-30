#include "LedgerServiceServer.h"
#include "../Clients/StorageServiceClient.h"
#include "StorageService.h"

using namespace bcostars;

void LedgerServiceServer::initialize() {
  auto storageServiceClient = std::make_shared<StorageServiceClient>(
      getApplication()->getCommunicator());
  auto ledger = std::make_shared<FakeLedger>();
  ledger->setStorageServiceClient(storageServiceClient);
  m_ledger = ledger;
}

void LedgerServiceServer::destroy() {}

tars::Int32
LedgerServiceServer::commitBlock(tars::Int64 blockNumber,
                                 const vector<vector<tars::Char>> &signList,
                                 tars::TarsCurrentPtr current) {}

tars::Int32
LedgerServiceServer::getBlockByHash(const vector<tars::Char> &blockHash,
                                    bcostars::Block &block,
                                    tars::TarsCurrentPtr current) {}

tars::Int32
LedgerServiceServer::getBlockByNumber(tars::Int64 blockNumber,
                                      bcostars::Block &block,
                                      tars::TarsCurrentPtr current) {
  current->setResponse(false);
  m_ledger->asyncGetTransactionsByBlockNumber(
      blockNumber, [current](bcos::Error::Ptr error,
                             bcos::protocol::TransactionsConstPtr txs) {
        tars::Int32 ret = 0;
        bcostars::Block block;
        block.blockHeader.blockNumber = 88980;

        std::cout << "getBlockByNumber" << std::endl;
 
        async_response_getBlockByNumber(current, ret, block);
      });
}

tars::Int32
LedgerServiceServer::getBlockHashByNumber(tars::Int64 blockNumber,
                                          vector<tars::Char> &blockHash,
                                          tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceServer::getBlockHeaderByHash(
    const vector<tars::Char> &blockHash, bcostars::BlockHeader &blockHeader,
    vector<vector<tars::Char>> &sigList, tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceServer::getBlockHeaderByNumber(
    tars::Int64 blockNumber, bcostars::BlockHeader &blockHeader,
    vector<vector<tars::Char>> &sigList, tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceServer::getBlockNumber(tars::Int64 &blockNumber,
                                                tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceServer::getCode(const std::string &tableID,
                                         const vector<tars::Char> &codeAddress,
                                         vector<tars::Char> &code,
                                         tars::TarsCurrentPtr current) {}

tars::Int32
LedgerServiceServer::getNonceList(tars::Int64 blockNumber,
                                  const vector<tars::Char> &nonceList,
                                  tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceServer::getTotalTransactionCount(
    tars::Int64 &totalTx, tars::Int64 &failedTx, tars::Int64 &blockNumber,
    tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceServer::getTransactionByBlockHashAndIndex(
    const vector<tars::Char> &blockHash, tars::Int64 index,
    bcostars::Transaction &tx, tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceServer::getTransactionByBlockNumberAndIndex(
    tars::Int64 blockNumber, tars::Int64 index, bcostars::Transaction &tx,
    tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceServer::getTransactionProof(
    const vector<tars::Char> &blockHash, tars::Int64 index,
    vector<std::string> &proof, tars::TarsCurrentPtr current) {}

tars::Int32
LedgerServiceServer::getTransactionReceiptByHash(const vector<tars::Char> &hash,
                                                 bcostars::Transaction &tx,
                                                 tars::TarsCurrentPtr current) {
}

tars::Int32 LedgerServiceServer::getTransactionReceiptProof(
    const vector<tars::Char> &block, tars::Int64 index,
    vector<std::string> &proof, tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceServer::getTxByHash(const vector<tars::Char> &hash,
                                             bcostars::Transaction &tx,
                                             tars::TarsCurrentPtr current) {}

tars::Int32
LedgerServiceServer::preStoreTxs(const vector<bcostars::Block> &blocks,
                                 tars::TarsCurrentPtr current) {}