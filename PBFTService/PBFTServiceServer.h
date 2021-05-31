#pragma once

#include "../Common/ErrorConverter.h"
// #include "../FrontService/FrontServiceClient.h"
#include "../protocols/BlockImpl.h"
#include "PBFTService.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include "bcos-framework/interfaces/front/FrontServiceInterface.h"
#include "bcos-framework/interfaces/protocol/BlockFactory.h"
#include "bcos-framework/interfaces/sealer/SealerInterface.h"
#include "bcos-framework/interfaces/storage/StorageInterface.h"
#include "bcos-framework/libprotocol/TransactionSubmitResultFactoryImpl.h"
#include "bcos-framework/libsealer/Sealer.h"
#include "bcos-framework/libsealer/SealerFactory.h"
#include "bcos-ledger/ledger/Ledger.h"
#include "bcos-pbft/pbft/PBFTFactory.h"
#include "bcos-txpool/TxPoolFactory.h"

namespace bcostars {

class PBFTServiceServer : public bcostars::PBFTService {
public:
  void initialize() override {
    // Params from config
    std::string groupID;
    std::string chainID;
    int64_t blockLimit;
    bcos::crypto::KeyPairInterface::Ptr keyPair;

    std::scoped_lock<std::mutex> scoped(m_initLock);
    if (m_sealer) {
      auto txSubmitResultFactory = std::make_shared<
          bcos::protocol::TransactionSubmitResultFactoryImpl>();
      auto blockFactory = std::make_shared<bcostars::protocol::BlockFactory>();
      bcos::front::FrontServiceInterface::Ptr
          frontService;                             // TODO: Init the client
      bcos::storage::StorageInterface::Ptr storage; // TODO: Init the storage
      bcos::dispatcher::DispatcherInterface::Ptr
          dispatcher; // TODO: Init the dispatcher
      auto ledger =
          std::make_shared<bcos::ledger::Ledger>(blockFactory, storage);

      auto txPoolFactory = std::make_shared<bcos::txpool::TxPoolFactory>(
          bcos::crypto::NodeIDPtr(), m_cryptoSuite, txSubmitResultFactory,
          blockFactory, frontService, ledger, groupID, chainID, blockLimit);

      auto sealerFactory = std::make_shared<bcos::sealer::SealerFactory>(
          blockFactory, txPoolFactory->txpool(), blockLimit);

      auto pbftFactory = std::make_shared<bcos::consensus::PBFTFactory>(
          m_cryptoSuite, keyPair, frontService, storage, ledger,
          txPoolFactory->txpool(), sealerFactory->sealer(), dispatcher,
          blockFactory);

      txPoolFactory->init(sealerFactory->sealer());
      sealerFactory->init(pbftFactory->consensus());
      pbftFactory->init();

      m_sealer = sealerFactory->sealer();
    }
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
  bcos::sealer::SealerInterface::Ptr m_sealer;
  static std::mutex m_initLock;
  bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
};

} // namespace bcostars