#pragma once

#include "../Common/ErrorConverter.h"
#include "PBFTService.h"
#include "bcos-framework/interfaces/sealer/SealerInterface.h"

namespace bcostars {
class PBFTServiceClient : public bcos::sealer::SealerInterface {
public:
  PBFTServiceClient(bcostars::PBFTServicePrx proxy) : m_proxy(proxy) {}
  ~PBFTServiceClient() override {}

  void start() override {}
  void stop() override {}

  void asyncNotifySealProposal(
      size_t _proposalIndex, size_t _proposalEndIndex, size_t _maxTxsToSeal,
      std::function<void(bcos::Error::Ptr)> onRecvResponse) override {
    class Callback : public bcostars::PBFTServicePrxCallback {
    public:
      Callback(std::function<void(bcos::Error::Ptr)> callback)
          : m_callback(callback) {}

      void
      callback_asyncNotifySealProposal(const bcostars::Error &ret) override {
        m_callback(toBcosError(ret));
      }
      void
      callback_asyncNotifySealProposal_exception(tars::Int32 ret) override {
        m_callback(toBcosError(ret));
      }

    private:
      std::function<void(bcos::Error::Ptr)> m_callback;
    };

    m_proxy->async_asyncNotifySealProposal(new Callback(onRecvResponse),
                                           _proposalIndex, _proposalEndIndex,
                                           _maxTxsToSeal);
  }

  void asyncNoteUnSealedTxsSize(
      size_t _unsealedTxsSize,
      std::function<void(bcos::Error::Ptr)> _onRecvResponse) override {
    class Callback : public bcostars::PBFTServicePrxCallback {
    public:
      Callback(std::function<void(bcos::Error::Ptr)> callback)
          : m_callback(callback) {}

      void
      callback_asyncNoteUnSealedTxsSize(const bcostars::Error &ret) override {
        m_callback(toBcosError(ret));
      }
      void
      callback_asyncNoteUnSealedTxsSize_exception(tars::Int32 ret) override {
        m_callback(toBcosError(ret));
      }

    private:
      std::function<void(bcos::Error::Ptr)> m_callback;
    };

    m_proxy->async_asyncNoteUnSealedTxsSize(new Callback(_onRecvResponse),
                                            _unsealedTxsSize);
  }

private:
  bcostars::PBFTServicePrx m_proxy;
};
} // namespace bcostars