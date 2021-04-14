#ifndef _LedgerImp_H_
#define _LedgerImp_H_

#include "servant/Application.h"
#include "Ledger.h"

/**
 *
 *
 */
class LedgerImp : public bcos::Ledger
{
public:
    /**
     *
     */
    virtual ~LedgerImp() override {}

    /**
     *
     */
    virtual void initialize() override;

    /**
     *
     */
    virtual void destroy() override;

    virtual void commitBlock(tars::Int32 blockNumber,tars::TarsCurrentPtr current) override;
};
/////////////////////////////////////////////////////
#endif
