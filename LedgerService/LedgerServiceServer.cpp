#include "LedgerServiceServer.h"
#include "../Clients/StorageServiceClient.h"
#include "StorageService.h"

using namespace bcostars;

void LedgerServiceServer::initialize() {
}

void LedgerServiceServer::destroy() {}

tars::Int32 LedgerServiceServer::test(tars::TarsCurrentPtr current) {
    return 0;
}