#pragma once

#include "DispatcherService.h"
#include "bcos-dispatcher/DispatcherImpl.h"
#include "../protocols/BlockImpl.h"
#include "../Common/ErrorConverter.h"
#include "bcos-crypto/hash/SM3.h"
#include "bcos-crypto/signature/sm2/SM2Crypto.h"
#include <memory>

namespace bcostars {

class DispatcherServiceServer : public DispatcherService {
public:
  void initialize() override {
    std::call_once(m_initFlag, []() {
      auto cryptoSuite =
          std::make_shared<bcos::crypto::CryptoSuite>(std::make_shared<bcos::crypto::SM3>(), std::make_shared<bcos::crypto::SM2Crypto>(), nullptr);

      auto transactionFactory = std::make_shared<bcostars::protocol::TransactionFactoryImpl>(cryptoSuite);
      auto transactionReceiptFactory = std::make_shared<bcostars::protocol::TransactionReceiptFactoryImpl>(cryptoSuite);

      m_blockHeaderFactory = std::make_shared<bcostars::protocol::BlockHeaderFactoryImpl>(cryptoSuite);
      m_blockFactory = std::make_shared<bcostars::protocol::BlockFactoryImpl>(cryptoSuite, m_blockHeaderFactory, transactionFactory, transactionReceiptFactory);
      m_dispatcher = std::make_shared<bcos::dispatcher::DispatcherImpl>();
    });
  }

  void destroy() override {}

  bcostars::Error asyncExecuteBlock(const bcostars::Block &block, tars::Bool verify, bcostars::BlockHeader &blockHeader,
                                    tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    auto bcosBlock = m_blockFactory->createBlock();
    std::dynamic_pointer_cast<bcostars::protocol::BlockImpl>(bcosBlock)->setInner(block);
    m_dispatcher->asyncExecuteBlock(bcosBlock, verify, [current](const bcos::Error::Ptr &error, const bcos::protocol::BlockHeader::Ptr &blockHeader) {
      async_response_asyncExecuteBlock(current, toTarsError(error), std::dynamic_pointer_cast<bcostars::protocol::BlockHeaderImpl>(blockHeader)->inner());
    });
    return bcostars::Error();
  }
  
  bcostars::Error asyncGetLatestBlock(bcostars::Block &block, tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    m_dispatcher->asyncGetLatestBlock([current](const bcos::Error::Ptr &error, const bcos::protocol::Block::Ptr &block) {
      async_response_asyncGetLatestBlock(current, toTarsError(error), std::dynamic_pointer_cast<bcostars::protocol::BlockImpl>(block)->inner());
    });

    return bcostars::Error();
  }

  bcostars::Error asyncNotifyExecutionResult(const bcostars::Error &error, const bcostars::BlockHeader &blockHeader, tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    auto bcosBlockHeader = m_blockHeaderFactory->createBlockHeader();
    std::dynamic_pointer_cast<bcostars::protocol::BlockHeaderImpl>(bcosBlockHeader)->setInner(blockHeader);
    m_dispatcher->asyncNotifyExecutionResult(toBcosError(error), bcosBlockHeader, [current](const bcos::Error::Ptr &error) {
      async_response_asyncNotifyExecutionResult(current, toTarsError(error));
    });
    return bcostars::Error();
  }

private:
  static std::once_flag m_initFlag;
  static bcos::dispatcher::DispatcherImpl::Ptr m_dispatcher;
  static bcostars::protocol::BlockHeaderFactoryImpl::Ptr m_blockHeaderFactory;
  static bcostars::protocol::BlockFactoryImpl::Ptr m_blockFactory;
};

} // namespace bcostars