#pragma once

#include "LedgerService.h"
#include "servant/Application.h"
#include <bcos-framework/interfaces/ledger/LedgerInterface.h>
#include "../StorageService/StorageServiceClient.h"

namespace bcostars {

class LedgerServiceServer : public LedgerService {
public:
  ~LedgerServiceServer() override {}

  void initialize() override;

  void destroy() override;

  tars::Int32 test(tars::TarsCurrentPtr current) override;

private:
  std::shared_ptr<bcos::ledger::LedgerInterface> m_ledger;
  bcostars::StorageServiceClient::Ptr m_storageServiceClient;
};

} // namespace bcostars
/////////////////////////////////////////////////////
