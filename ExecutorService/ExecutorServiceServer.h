#pragma once

#include "ExecutorService.h"
#include "../Common/ErrorConverter.h"
#include "../protocols/TransactionImpl.h"
#include "../protocols/TransactionReceiptImpl.h"
#include "bcos-framework/interfaces/executor/ExecutorInterface.h"
#include <memory>
#include <mutex>

namespace bcostars {
class ExecutorServiceServer : public bcostars::ExecutorService {
public:
void initialize() override {
    std::call_once(m_initFlag, [this]() {

    });
  }

  bcostars::Error asyncExecuteTransaction(const bcostars::Transaction &transaction, bcostars::TransactionReceipt &receipt,
                                          tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    auto bcosTransaction = std::make_shared<bcostars::protocol::TransactionImpl>(&transaction, m_cryptoSuite);
    m_executor->asyncExecuteTransaction(bcosTransaction, [current](const bcos::Error::Ptr &error, const bcos::protocol::TransactionReceipt::ConstPtr &receipt) {
      if (error && error->errorCode()) {
        bcostars::TransactionReceipt nullobj;
        async_response_asyncExecuteTransaction(current, toTarsError(error), nullobj);
        return;
      }

      async_response_asyncExecuteTransaction(current, toTarsError(error),
                                             std::dynamic_pointer_cast<bcostars::protocol::TransactionReceiptImpl>(receipt)->inner());
    });
  }

  bcostars::Error asyncGetCode(const std::string &address, vector<tars::UInt8> &code, tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    m_executor->asyncGetCode(std::make_shared<std::string>(address), [current](const bcos::Error::Ptr &error, const std::shared_ptr<bcos::bytes> &code) {
      if (error && error->errorCode()) {
        vector<tars::UInt8> nullobj;
        async_response_asyncGetCode(current, toTarsError(error), nullobj);
        return;
      }

      async_response_asyncGetCode(current, toTarsError(error), *code);
    });
  }

private:
  static std::once_flag m_initFlag;
  static std::shared_ptr<bcos::executor::ExecutorInterface> m_executor;
  static bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};
} // namespace bcostars