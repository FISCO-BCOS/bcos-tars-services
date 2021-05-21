#pragma once

#include "StorageService.h"
#include "servant/Application.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <exception>
#include <type_traits>

namespace bcostars {
class StorageServiceClient : public bcos::storage::StorageInterface {
public:
  ~StorageServiceClient() override {}

  StorageServiceClient(CommunicatorPtr communicator)
      : m_communicator(communicator) {}

  std::vector<std::string> getPrimaryKeys(
      std::shared_ptr<bcos::storage::TableInfo> _tableInfo,
      std::shared_ptr<bcos::storage::Condition> _condition) const override {
        /*
    bcostars::TableInfo tableInfo;
    tableInfo.name = _tableInfo->name;
    tableInfo.keyValue = _tableInfo->key;
    tableInfo.fields = _tableInfo->fields;
    tableInfo.indices = _tableInfo->indices;
    tableInfo.enableConsensus = _tableInfo->enableConsensus;

    bcostars::Condition condition;
    (void)_condition;

    try {
      auto proxy =
          m_communicator->stringToProxy<StorageServicePrx>(m_servantName);
      return proxy->getPrimaryKeys(tableInfo, condition);
    } catch (std::exception &e) {
      throw e;
    }
    */
  };

private:
  CommunicatorPtr m_communicator;
  const std::string m_servantName = "bcostars.StorageService.StorageServiceObj";
};
} // namespace bcostars