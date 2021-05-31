#pragma once

#include "FrontService.h"
#include "bcos-framework/interfaces/front/FrontServiceInterface.h"

namespace bcostars {
class FrontServiceClient : public bcos::front::FrontServiceInterface {
public:
  void start() override {}
  void stop() override {}

  void asyncGetNodeIDs(bcos::front::GetNodeIDsFunc _getNodeIDsFunc) override {
  }

  void
  onReceiveNodeIDs(const std::string &_groupID,
                   std::shared_ptr<const bcos::crypto::NodeIDs> _nodeIDs,
                   bcos::front::ReceiveMsgFunc _receiveMsgCallback) override {}

  void
  onReceiveMessage(const std::string &_groupID, bcos::crypto::NodeIDPtr _nodeID,
                   bcos::bytesConstRef _data,
                   bcos::front::ReceiveMsgFunc _receiveMsgCallback) override {}

  void onReceiveBroadcastMessage(
      const std::string &_groupID, bcos::crypto::NodeIDPtr _nodeID,
      bcos::bytesConstRef _data,
      bcos::front::ReceiveMsgFunc _receiveMsgCallback) override {}

  void asyncSendMessageByNodeID(int _moduleID, bcos::crypto::NodeIDPtr _nodeID,
                                bcos::bytesConstRef _data, uint32_t _timeout,
                                bcos::front::CallbackFunc _callback) override {}

  void asyncSendResponse(const std::string &_id,
                         bcos::bytesConstRef _data) override {}

  void asyncSendMessageByNodeIDs(
      int _moduleID, const std::vector<bcos::crypto::NodeIDPtr> &_nodeIDs,
      bcos::bytesConstRef _data) override {}

  void asyncSendBroadcastMessage(int _moduleID,
                                 bcos::bytesConstRef _data) override {}

private:
  bcostars::FrontServicePrx m_proxy;
};
} // namespace bcostars