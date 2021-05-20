#include "StorageServiceServer.h"
#include <bcos-framework/interfaces/storage/Common.h>
#include <bcos-framework/interfaces/storage/StorageInterface.h>

using namespace bcostars;

void StorageServiceServer::initialize() {}

void StorageServiceServer::destroy() {}

vector<std::string>
StorageServiceServer::getPrimaryKeys(const bcostars::TableInfo &tableInfo,
                                     const bcostars::Condition &condition,
                                     tars::TarsCurrentPtr current) {
}