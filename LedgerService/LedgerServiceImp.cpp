#include "LedgerServiceImp.h"

using namespace bcostars;

void LedgerServiceImp::initialize() {}

void LedgerServiceImp::destroy() {}

tars::Int32
LedgerServiceImp::commitBlock(tars::Int64 blockNumber,
                              const vector<vector<tars::Char>> &signList,
                              tars::TarsCurrentPtr current) {}

tars::Int32
LedgerServiceImp::getBlockByHash(const vector<tars::Char> &blockHash,
                                 bcostars::Block &block,
                                 tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getBlockByNumber(tars::Int64 blockNumber,
                                               bcostars::Block &block,
                                               tars::TarsCurrentPtr current) {}

tars::Int32
LedgerServiceImp::getBlockHashByNumber(tars::Int64 blockNumber,
                                       vector<tars::Char> &blockHash,
                                       tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getBlockHeaderByHash(
    const vector<tars::Char> &blockHash, bcostars::BlockHeader &blockHeader,
    vector<vector<tars::Char>> &sigList, tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getBlockHeaderByNumber(
    tars::Int64 blockNumber, bcostars::BlockHeader &blockHeader,
    vector<vector<tars::Char>> &sigList, tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getBlockNumber(tars::Int64 &blockNumber,
                                             tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getCode(const std::string &tableID,
                                      const vector<tars::Char> &codeAddress,
                                      vector<tars::Char> &code,
                                      tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getNonceList(tars::Int64 blockNumber,
                                           const vector<tars::Char> &nonceList,
                                           tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getTotalTransactionCount(
    tars::Int64 &totalTx, tars::Int64 &failedTx, tars::Int64 &blockNumber,
    tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getTransactionByBlockHashAndIndex(
    const vector<tars::Char> &blockHash, tars::Int64 index,
    bcostars::Transaction &tx, tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getTransactionByBlockNumberAndIndex(
    tars::Int64 blockNumber, tars::Int64 index, bcostars::Transaction &tx,
    tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getTransactionProof(
    const vector<tars::Char> &blockHash, tars::Int64 index,
    vector<std::string> &proof, tars::TarsCurrentPtr current) {}

tars::Int32
LedgerServiceImp::getTransactionReceiptByHash(const vector<tars::Char> &hash,
                                              bcostars::Transaction &tx,
                                              tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getTransactionReceiptProof(
    const vector<tars::Char> &block, tars::Int64 index,
    vector<std::string> &proof, tars::TarsCurrentPtr current) {}

tars::Int32 LedgerServiceImp::getTxByHash(const vector<tars::Char> &hash,
                                          bcostars::Transaction &tx,
                                          tars::TarsCurrentPtr current) {}
                                          
tars::Int32 LedgerServiceImp::preStoreTxs(const vector<bcostars::Block> &blocks,
                                          tars::TarsCurrentPtr current) {}