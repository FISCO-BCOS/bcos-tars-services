#pragma once

#include "FrontService.h"
#include "PBFTService.h"
#include "servant/Application.h"
#include "servant/Communicator.h"
#include "../Common/ErrorConverter.h"
#include "../FrontService/FrontServiceClient.h"
#include "../StorageService/StorageServiceClient.h"
#include "../protocols/BlockImpl.h"
#include "../protocols/TransactionImpl.h"
#include "../protocols/TransactionReceiptImpl.h"
#include "bcos-crypto/hash/SM3.h"
#include "bcos-crypto/signature/sm2/SM2Crypto.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include "bcos-framework/interfaces/front/FrontServiceInterface.h"
#include "bcos-framework/interfaces/protocol/BlockFactory.h"
#include "bcos-framework/interfaces/protocol/TransactionFactory.h"
#include "bcos-framework/interfaces/protocol/TransactionReceiptFactory.h"
#include "bcos-framework/interfaces/sealer/SealerInterface.h"
#include "bcos-framework/interfaces/storage/StorageInterface.h"
#include "bcos-framework/libprotocol/TransactionSubmitResultFactoryImpl.h"
#include "bcos-framework/libsealer/Sealer.h"
#include "bcos-framework/libsealer/SealerFactory.h"
#include "bcos-ledger/ledger/Ledger.h"
#include "bcos-pbft/pbft/PBFTFactory.h"
#include "bcos-txpool/TxPoolFactory.h"
#include <mutex>

namespace bcostars {

class PBFTServiceServer : public bcostars::PBFTService {
public:
  void initialize() override {
    std::call_once(m_pbftFlag, [this]() {
      // ---- Params from config
      std::string groupID;
      std::string chainID;
      int64_t blockLimit;
      bcos::crypto::KeyPairInterface::Ptr keyPair;
      std::string frontServiceDesc;
      std::string storageServiceDesc;
      // ------------------

      bcos::crypto::CryptoSuite::Ptr cryptoSuite = std::make_shared<bcos::crypto::CryptoSuite>(
          std::make_shared<bcos::crypto::SM3>(),
          std::make_shared<bcos::crypto::SM2Crypto>(), nullptr);

      bcos::protocol::BlockHeaderFactory::Ptr blockHeaderFactory =
          std::make_shared<bcostars::protocol::BlockHeaderFactoryImpl>(
              cryptoSuite);

      bcos::protocol::TransactionFactory::Ptr transactionFactory =
          std::make_shared<bcostars::protocol::TransactionFactoryImpl>(
              cryptoSuite);

      bcos::protocol::TransactionReceiptFactory::Ptr transactionReceiptFactory =
          std::make_shared<bcostars::protocol::TransactionReceiptFactoryImpl>(
              cryptoSuite);

      auto txSubmitResultFactory = std::make_shared<
          bcos::protocol::TransactionSubmitResultFactoryImpl>();
      auto blockFactory =
          std::make_shared<bcostars::protocol::BlockFactoryImpl>(
              cryptoSuite, blockHeaderFactory, transactionFactory,
              transactionReceiptFactory);

      bcostars::FrontServicePrx frontServiceProxy =
          Application::getCommunicator()
              ->stringToProxy<bcostars::FrontServicePrx>(frontServiceDesc);
      bcos::front::FrontServiceInterface::Ptr frontServiceClient =
          std::make_shared<bcostars::FrontServiceClient>(
              frontServiceProxy, m_cryptoSuite->keyFactory());

      bcostars::StorageServicePrx storageServiceProxy =
          Application::getCommunicator()
              ->stringToProxy<bcostars::StorageServicePrx>(storageServiceDesc);
      bcos::storage::StorageInterface::Ptr storageServiceClient =
          std::make_shared<bcostars::StorageServiceClient>(storageServiceProxy);

      bcos::dispatcher::DispatcherInterface::Ptr
          dispatcher; // TODO: Init the dispatcher

      auto ledger = std::make_shared<bcos::ledger::Ledger>(
          blockFactory, storageServiceClient);

      auto txPoolFactory = std::make_shared<bcos::txpool::TxPoolFactory>(
          bcos::crypto::NodeIDPtr(), m_cryptoSuite, txSubmitResultFactory,
          blockFactory, frontServiceClient, ledger, groupID, chainID,
          blockLimit);

      auto sealerFactory = std::make_shared<bcos::sealer::SealerFactory>(
          blockFactory, txPoolFactory->txpool(), blockLimit);

      auto pbftFactory = std::make_shared<bcos::consensus::PBFTFactory>(
          m_cryptoSuite, keyPair, frontServiceClient, storageServiceClient,
          ledger, txPoolFactory->txpool(), sealerFactory->sealer(), dispatcher,
          blockFactory);

      txPoolFactory->init(sealerFactory->sealer());
      sealerFactory->init(pbftFactory->consensus());
      pbftFactory->init();

      txPoolFactory->txpool()->start();
      sealerFactory->sealer()->start();
      pbftFactory->consensus()->start();

      m_sealer = sealerFactory->sealer();
    });
  }

  void destroy() override {}

  bcostars::Error
  asyncNoteUnSealedTxsSize(tars::Int64 unsealedTxsSize,
                           tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    m_sealer->asyncNoteUnSealedTxsSize(
        unsealedTxsSize, [current](bcos::Error::Ptr error) {
          async_response_asyncNoteUnSealedTxsSize(current, toTarsError(error));
        });
  }

  bcostars::Error asyncNotifySealProposal(
      tars::Int64 proposalIndex, tars::Int64 proposalEndIndex,
      tars::Int64 maxTxsToSeal, tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    m_sealer->asyncNotifySealProposal(
        proposalIndex, proposalEndIndex, maxTxsToSeal,
        [current](bcos::Error::Ptr error) {
          async_response_asyncNotifySealProposal(current, toTarsError(error));
        });
  }

private:
  static std::once_flag m_pbftFlag;
  static bcos::sealer::SealerInterface::Ptr m_sealer;
  static bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};

} // namespace bcostars