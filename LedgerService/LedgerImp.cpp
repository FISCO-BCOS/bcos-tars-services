#include "LedgerImp.h"
#include "servant/Application.h"

using namespace bcos_tars;

void LedgerImp::initialize()
{
}

void LedgerImp::destroy()
{
}

void LedgerImp::commitBlock(tars::Int32 blockNumber,tars::TarsCurrentPtr current) {
    current->sendResponse(0);
}