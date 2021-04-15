#include "Block.h"

using namespace bcos_tars::protocol;

Block::~Block() {}

void Block::decode(bcos::bytesConstRef _data, bool _calculateHash,
                       bool _checkSig){
                           // m_block.readFrom(tars::TarsInputStream<ReaderT> &_is)
                       }
void Block::encode(bcos::bytes &_encodeData) const{}
bcos::crypto::HashType
Block::calculateTransactionRoot(bool _updateHeader) const{}
bcos::crypto::HashType
Block::calculateReceiptRoot(bool _updateHeader) const{}

int32_t Block::version() const{}
void Block::setVersion(int32_t _version){}
// get blockHeader
bcos::protocol::BlockHeader::Ptr Block::blockHeader() const{}
// get transactions
bcos::protocol::TransactionsConstPtr Block::transactions(){}
bcos::protocol::Transaction::ConstPtr
Block::transaction(size_t _index){}
// get receipts
bcos::protocol::ReceiptsConstPtr Block::receipts(){}
bcos::protocol::TransactionReceipt::ConstPtr
Block::receipt(size_t _index){}
// get transaction hash
bcos::protocol::HashListConstPtr Block::transactionsHash(){}
bcos::crypto::HashType const &
Block::transactionHash(size_t _index){}
// get receipt hash
bcos::protocol::HashListConstPtr Block::receiptsHash(){}
bcos::crypto::HashType const &Block::receiptHash(size_t _index){}

void Block::setBlockType(bcos::protocol::BlockType _blockType){}
// set blockHeader
void Block::setBlockHeader(
    bcos::protocol::BlockHeader::Ptr _blockHeader){}
// set transactions
void Block::setTransactions(
    bcos::protocol::TransactionsPtr _transactions){}
void Block::setTransaction(
    size_t _index, bcos::protocol::Transaction::Ptr _transaction){}
void Block::appendTransaction(
    bcos::protocol::Transaction::Ptr _transaction){}
// set receipts
void Block::setReceipts(bcos::protocol::ReceiptsPtr _receipts){}
void Block::setReceipt(
    size_t _index, bcos::protocol::TransactionReceipt::Ptr _receipt){}
void Block::appendReceipt(
    bcos::protocol::TransactionReceipt::Ptr _receipt){}
// set transaction hash
void Block::setTransactionsHash(
    bcos::protocol::HashListPtr _transactionsHash){}
void Block::setTransactionHash(
    size_t _index, bcos::crypto::HashType const &_txHash){}
void Block::appendTransactionHash(
    bcos::crypto::HashType const &_txHash){}
// set receipt hash
void Block::setReceiptsHash(
    bcos::protocol::HashListPtr _receiptsHash){}
void Block::setReceiptHash(
    size_t _index, bcos::crypto::HashType const &_receptHash){}
void Block::appendReceiptHash(
    bcos::crypto::HashType const &_receiptHash){}
// getNonces of the current block
bcos::protocol::NonceListPtr Block::nonces(){}
// get transactions size
size_t Block::transactionsSize(){}
size_t Block::transactionsHashSize(){}
// get receipts size
size_t Block::receiptsSize(){}
size_t Block::receiptsHashSize(){}