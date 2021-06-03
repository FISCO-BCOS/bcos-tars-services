#pragma once

#include "../Common/ErrorConverter.h"
#include "FrontService.h"
#include "bcos-framework/interfaces/crypto/KeyFactory.h"
#include "bcos-framework/interfaces/crypto/KeyInterface.h"
#include "bcos-framework/interfaces/front/FrontServiceInterface.h"
#include <bcos-front/front/FrontService.h>
#include <bcos-front/front/FrontServiceFactory.h>
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>

namespace bcostars {
class FrontServiceServer : public FrontService {
  ~FrontServiceServer() override {}

  void initialize() override {
    std::call_once(m_onceFlag, [this]() {
      std::string groupID;
      bcos::crypto::NodeIDPtr nodeID;

      bcos::front::FrontServiceFactory frontServiceFactory;
      frontServiceFactory.setGroupID(groupID);
      frontServiceFactory.setNodeID(nodeID);
      frontServiceFactory.setGatewayInterface(
          nullptr); // TODO: set the gateway interface

      m_front = frontServiceFactory.buildFrontService();
    });

    m_keyFactory = std::make_shared<bcos::crypto::KeyFactoryImpl>();
  }

  void destroy() override {}

  bcostars::Error asyncGetNodeIDs(vector<vector<tars::UInt8>> &nodeIDs,
                                  tars::TarsCurrentPtr current) override {
    current->setResponse(false);

    m_front->asyncGetNodeIDs(
        [current](bcos::Error::Ptr _error,
                  std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs) {
          std::vector<bcos::bytes> tarsNodeIDs;
          tarsNodeIDs.reserve(_nodeIDs->size());
          for (auto const &it : *_nodeIDs) {
            tarsNodeIDs.push_back(it->data());
          }

          async_response_asyncGetNodeIDs(current, toTarsError(_error),
                                         tarsNodeIDs);
        });
  }

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
                  bcos::bytesConstRef _data, const std::string &_id,
                  bcos::front::ResponseFunc _respFunc) {
          async_response_asyncSendMessageByNodeID(current, toTarsError(_error),
                                                  *_nodeID->encode(),
                                                  _data.toBytes(), _id);
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
  static std::once_flag m_onceFlag;
  static bcos::front::FrontServiceInterface::Ptr m_front;
  bcos::crypto::KeyFactory::Ptr m_keyFactory;
};
} // namespace bcostars