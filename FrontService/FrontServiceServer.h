#pragma once

#include "../Common/ErrorConverter.h"
#include "FrontService.h"
#include "bcos-framework/interfaces/crypto/KeyFactory.h"
#include "bcos-framework/interfaces/crypto/KeyInterface.h"
#include "bcos-framework/interfaces/front/FrontServiceInterface.h"
// #include <bcos-front/front/FrontService.h>

namespace bcostars {
class FrontServiceServer : public FrontService {
  ~FrontServiceServer() override {}

  void initialize() override {}

  void destroy() override {}

  void asyncSendBroadcastMessage(tars::Int32 moduleID,
                                 const vector<tars::UInt8> &data,
                                 tars::TarsCurrentPtr current) override {
    m_front->asyncSendBroadcastMessage(moduleID, bcos::ref(data));
  }

  bcostars::Error asyncSendMessageByNodeID(
      tars::Int32 moduleID, const vector<tars::UInt8> &nodeID,
      const vector<tars::UInt8> &data, tars::UInt32 timeout,
      vector<tars::UInt8> &responseNodeID, vector<tars::UInt8> &responseData,
      std::string &seq, tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    auto bcosNodeID = m_keyFactory->createKey(nodeID);
    m_front->asyncSendMessageByNodeID(
        moduleID, bcosNodeID, bcos::ref(data), timeout,
        [current](bcos::Error::Ptr _error, bcos::crypto::NodeIDPtr _nodeID,
                  bcos::bytesConstRef _data,
                  bcos::front::ResponseFunc _respFunc) {
          async_response_asyncSendMessageByNodeID(
              current, toTarsError(_error), *_nodeID->encode(), _data.toBytes(),
              std::string()); // TODO: Add seq
        });
  }

  void asyncSendMessageByNodeIDs(tars::Int32 moduleID,
                                 const vector<vector<tars::UInt8>> &nodeIDs,
                                 const vector<tars::UInt8> &data,
                                 tars::TarsCurrentPtr current) override {
    std::vector<bcos::crypto::NodeIDPtr> bcosNodeIDs;
    bcosNodeIDs.reserve(nodeIDs.size());
    for (auto const &it : nodeIDs) {
      bcosNodeIDs.push_back(m_keyFactory->createKey(it));
    }

    m_front->asyncSendMessageByNodeIDs(moduleID, bcosNodeIDs, bcos::ref(data));
  }

  bcostars::Error asyncSendResponse(const std::string &seq,
                                    const vector<tars::UInt8> &data,
                                    tars::TarsCurrentPtr current) override {
    // TODO: use async send response interface
  }

  bcostars::Error onReceiveBroadcastMessage(
      const std::string &groupID, const vector<tars::UInt8> &nodeID,
      const vector<tars::UInt8> &data, tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    m_front->onReceiveBroadcastMessage(
        groupID, m_keyFactory->createKey(nodeID), bcos::ref(data),
        [current](bcos::Error::Ptr error) {
          async_response_onReceiveBroadcastMessage(current, toTarsError(error));
        });
  }

  bcostars::Error onReceiveMessage(const std::string &groupID,
                                   const vector<tars::UInt8> &nodeID,
                                   const vector<tars::UInt8> &data,
                                   tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    m_front->onReceiveMessage(
        groupID, m_keyFactory->createKey(nodeID), bcos::ref(data),
        [current](bcos::Error::Ptr error) {
          async_response_onReceiveMessage(current, toTarsError(error));
        });
  }

  bcostars::Error onReceivedNodeIDs(const std::string &groupID,
                                    const vector<vector<tars::UInt8>> &nodeIDs,
                                    tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    auto bcosNodeIDs = std::make_shared<std::vector<bcos::crypto::NodeIDPtr>>();
    bcosNodeIDs->reserve(nodeIDs.size());

    for (auto const &it : nodeIDs) {
      bcosNodeIDs->push_back(m_keyFactory->createKey(it));
    }

    m_front->onReceiveNodeIDs(
        groupID, bcosNodeIDs, [current](bcos::Error::Ptr error) {
          async_response_onReceivedNodeIDs(current, toTarsError(error));
        });
  }

private:
  bcos::front::FrontServiceInterface::Ptr m_front;
  bcos::crypto::KeyFactory::Ptr m_keyFactory;
};
} // namespace bcostars