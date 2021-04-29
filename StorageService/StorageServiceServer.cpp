#include "StorageServiceServer.h"
#include <bcos-framework/interfaces/storage/Common.h>

using namespace bcostars;

void StorageServiceServer::initialize() {}

void StorageServiceServer::destroy() {}

tars::Int32 StorageServiceServer::getPrimaryKeys(
    const bcostars::TableInfo &tableInfo, const bcostars::Condition &condition,
    vector<std::string> &keys, tars::TarsCurrentPtr current) {
        current->setResponse(false);

        auto bcosTableInfo = std::make_shared<bcos::storage::TableInfo>(tableInfo.name, tableInfo.keyValue);
        auto bcosCondition = std::make_shared<bcos::storage::Condition>();

        m_storage->asyncGetPrimaryKeys(bcosTableInfo, bcosCondition, [current](bcos::Error error, std::vector<std::string> result) {
        });
    }