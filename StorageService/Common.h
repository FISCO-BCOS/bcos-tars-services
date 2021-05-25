#pragma once

#include "StorageService.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <bcos-framework/interfaces/storage/TableInterface.h>
#include <bcos-framework/libtable/TableFactory.h>

namespace bcostars {
inline Error toTarsError(const bcos::Error &error) {
  Error tarsError;
  tarsError.errorCode = error.errorCode();
  tarsError.errorMessage = error.errorMessage();

  return tarsError;
}

inline Error toTarsError(const bcos::Error::Ptr &error) {
  Error tarsError;

  if (error) {
    tarsError.errorCode = error->errorCode();
    tarsError.errorMessage = error->errorMessage();
  }

  return tarsError;
}

inline bcos::storage::TableInfo::Ptr
toBcosTableInfo(const bcostars::TableInfo &tableInfo) {
  return std::make_shared<bcos::storage::TableInfo>(
      tableInfo.name, tableInfo._key, tableInfo.fields);
}

inline bcostars::TableInfo
toTarsTableInfo(const bcos::storage::TableInfo::Ptr &tableInfo) {
  bcostars::TableInfo tarsTableInfo;
  tarsTableInfo.name = tableInfo->name;
  tarsTableInfo._key = tableInfo->key;
  tarsTableInfo.fields = tableInfo->fields;

  return tarsTableInfo;
}

inline bcos::storage::Entry::Ptr toBcosEntry(const bcostars::Entry &entry) {
  auto bcosEntry = std::make_shared<bcos::storage::Entry>();
  bcosEntry->setNum(entry.num);
  bcosEntry->setStatus((bcos::storage::Entry::Status)entry.status);
  for (auto const &field : entry.fields) {
    bcosEntry->setField(field.first, field.second);
  }

  return bcosEntry;
}

inline bcostars::Entry toTarsEntry(const bcos::storage::Entry::Ptr &entry) {
  bcostars::Entry tarsEntry;
  tarsEntry.num = entry->num();
  tarsEntry.status = entry->getStatus();
  for (auto const &field : *entry) {
    tarsEntry.fields.emplace(field);
  }

  return tarsEntry;
}

inline bcos::storage::TableFactoryInterface::Ptr
toBcosTableFactory(const bcostars::TableFactory &tableFactory,
                   bcos::storage::StorageInterface::Ptr storage,
                   bcos::crypto::CryptoSuite::Ptr cryptoSuite) {
  bcos::storage::TableFactoryInterface::Ptr bcosTableFactory =
      std::make_shared<bcos::storage::TableFactory>(
          storage, cryptoSuite->hashImpl(), tableFactory.num);

  std::vector<bcos::storage::TableInfo::Ptr> tableInfos;
  for (auto const &tableInfo : tableFactory.tableInfos) {
    tableInfos.emplace_back(toBcosTableInfo(tableInfo));
  }

  std::vector<std::shared_ptr<std::map<std::string, bcos::storage::Entry::Ptr>>>
      tableDatas;
  for (auto const &tableData : tableFactory.datas) {
    auto bcosTableData =
        std::make_shared<std::map<std::string, bcos::storage::Entry::Ptr>>();
    for (auto const &entry : tableData) {
      auto bcosEntry = std::make_shared<bcos::storage::Entry>();
      bcosEntry->setNum(entry.second.num);
      bcosEntry->setStatus((bcos::storage::Entry::Status)entry.second.status);
      for (auto &field : entry.second.fields) {
        bcosEntry->setField(std::move(field.first), std::move(field.second));
      }
      bcosTableData->emplace(entry.first, bcosEntry);
    }
    tableDatas.emplace_back(bcosTableData);
  }

  bcosTableFactory->importData(tableInfos, tableDatas);

  return bcosTableFactory;
}

inline bcostars::TableFactory toTarsTableFactory(
    const bcos::storage::TableFactoryInterface::Ptr &tableFactory) {
  bcostars::TableFactory tarsTableFactory;

  tarsTableFactory.num = tableFactory->blockNumber();
  auto tableDatas = tableFactory->exportData();

  for (auto const &tableInfo : tableDatas.first) {
    bcostars::TableInfo tarsTableInfo;
    tarsTableInfo.name = tableInfo->name;
    tarsTableInfo.fields = tableInfo->fields;
    tarsTableInfo._key = tableInfo->key;

    tarsTableFactory.tableInfos.emplace_back(tarsTableInfo);
  }
}

inline bcos::Error::Ptr toBcosError(const bcostars::Error &error) {
  if (error.errorCode == 0) {
    return nullptr;
  }

  auto bcosError =
      std::make_shared<bcos::Error>(error.errorCode, error.errorMessage);
  return bcosError;
}

inline bcos::Error::Ptr toBcosError(tars::Int32 ret) {
  if (ret == 0) {
    return nullptr;
  }

  auto bcosError = std::make_shared<bcos::Error>(ret, "TARS error!");
  return bcosError;
}

inline bcostars::Condition
toTarsCondition(const bcos::storage::Condition::Ptr &condition) {
  bcostars::Condition tarsCondition;
  tarsCondition.offset = condition->getLimit().first;
  tarsCondition.size = condition->getLimit().second;
  for (auto const &it : condition->m_conditions) {
    tarsCondition.conditions.push_back((short)(it.cmp));
  }

  return tarsCondition;
}

} // namespace bcostars