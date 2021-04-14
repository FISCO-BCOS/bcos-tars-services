#pragma once

#include "Ledger.h"
#include "servant/Application.h"

namespace bcos_tars {

class LedgerImp : public Ledger {
public:
  virtual ~LedgerImp() override {}

  virtual void initialize() override;

  virtual void destroy() override;

  virtual void commitBlock(tars::Int32 blockNumber,
                           tars::TarsCurrentPtr current) override;
};

} // namespace bcos
/////////////////////////////////////////////////////
