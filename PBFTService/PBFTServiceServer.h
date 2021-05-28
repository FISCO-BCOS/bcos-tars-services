#pragma once

#include "../Common/ErrorConverter.h"
#include "PBFTService.h"
#include "bcos-framework/interfaces/sealer/SealerInterface.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include "bcos-framework/libsealer/Sealer.h"
#include "bcos-pbft/pbft/PBFTFactory.h"
// #include "bcos-framework/libsealer/SealerFactory.h"

namespace bcostars {

class PBFTServiceServer : public bcostars::PBFTService {
public:
  void initialize() override {
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