#pragma once

#include "TxPoolService.h"
#include "bcos-txpool/TxPool.h"
#include "interfaces/crypto/Hash.h"
#include "libutilities/Common.h"
#include "../protocols/TransactionSubmitResultImpl.h"
#include "../Common/ErrorConverter.h"
#include <memory>

namespace bcostars {
class TxPoolServiceServer : public bcostars::TxPoolService {
public:
  bcostars::Error asyncFillBlock(const vector<vector<tars::UInt8>> &txHashs, vector<bcostars::Transaction> &filled, tars::TarsCurrentPtr current) override {
    current->setResponse(false);
    auto hashList = std::make_shared<std::vector<bcos::crypto::Hash>>();
    for (auto hashData : txHashs) {
        bcos::crypto::Hash a;
      hashList->push_back(hash);
    }

    // m_txpool->asyncFillBlock(bcos::crypto::HashListPtr _txsHash, std::function<void (Error::Ptr, bcos::protocol::TransactionsPtr)> _onBlockFilled)
  }

  bcostars::Error asynSubmit(const vector<tars::UInt8> &tx, bcostars::TransactionSubmitResult &result, tars::TarsCurrentPtr current) override {
    current->setResponse(false);
    auto dataPtr = std::make_shared<bcos::bytes>(tx);
    m_txpool->asyncSubmit(
        dataPtr,
        [current](bcos::Error::Ptr error, bcos::protocol::TransactionSubmitResult::Ptr result) {
          async_response_asyncSubmit(current, toTarsError(error), std::dynamic_pointer_cast<bcostars::protocol::TransactionSubmitResultImpl>(result)->inner());
        },
        [current](bcos::Error::Ptr error) {});

    return bcostars::Error();
  }

private:
  bcos::txpool::TxPool::Ptr m_txpool;
  bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
} // namespace bcostars