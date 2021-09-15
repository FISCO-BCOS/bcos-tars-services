#pragma once

#include "../Common/ErrorConverter.h"
#include "bcos-framework/interfaces/crypto/CryptoSuite.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <bcos-framework/interfaces/storage/TableInterface.h>
#include <bcos-framework/libtable/TableFactory.h>
#include <bcos-tars-protocol/StorageService.h>

namespace bcostars
{
inline bcos::storage::TableInfo::Ptr toBcosTableInfo(const bcostars::TableInfo& tableInfo)
{
    return std::make_shared<bcos::storage::TableInfo>(
        tableInfo.name, tableInfo._key, tableInfo.fields);
}

inline bcostars::TableInfo toTarsTableInfo(const bcos::storage::TableInfo::Ptr& tableInfo)
{
    bcostars::TableInfo tarsTableInfo;
    tarsTableInfo.name = tableInfo->name;
    tarsTableInfo._key = tableInfo->key;
    tarsTableInfo.fields = tableInfo->fields;

    return tarsTableInfo;
}

inline bcos::storage::Entry::Ptr toBcosEntry(const bcostars::Entry& entry)
{
    if (entry.isNull)
    {
        return nullptr;
    }
    auto bcosEntry = std::make_shared<bcos::storage::Entry>();
    bcosEntry->setNum(entry.num);
    bcosEntry->setStatus((bcos::storage::Entry::Status)entry.status);
    for (auto const& field : entry.fields)
    {
        bcosEntry->setField(field.first, field.second);
    }

    return bcosEntry;
}

inline bcostars::Entry toTarsEntry(const bcos::storage::Entry::Ptr& entry)
{
    bcostars::Entry tarsEntry;
    if (!entry)
    {
        tarsEntry.isNull = true;
        return tarsEntry;
    }
    tarsEntry.isNull = false;
    tarsEntry.num = entry->num();
    tarsEntry.status = entry->getStatus();
    for (auto const& field : *entry)
    {
        tarsEntry.fields.emplace(field);
    }

    return tarsEntry;
}

inline bcos::storage::TableFactoryInterface::Ptr toBcosTableFactory(
    const bcostars::TableFactory& tableFactory, bcos::storage::StorageInterface::Ptr storage,
    bcos::crypto::CryptoSuite::Ptr cryptoSuite)
{
    bcos::storage::TableFactoryInterface::Ptr bcosTableFactory =
        std::make_shared<bcos::storage::TableFactory>(
            storage, cryptoSuite->hashImpl(), tableFactory.num);

    // convert tars tableInfo to bcos tableInfo
    std::vector<bcos::storage::TableInfo::Ptr> tableInfos;
    for (auto const& tableInfo : tableFactory.tableInfos)
    {
        tableInfos.emplace_back(toBcosTableInfo(tableInfo));
    }

    // convert tars tableData to bcos tableData
    std::vector<std::shared_ptr<std::map<std::string, bcos::storage::Entry::Ptr>>> tableDatas;
    for (auto const& tableData : tableFactory.datas)
    {
        auto bcosTableData = std::make_shared<std::map<std::string, bcos::storage::Entry::Ptr>>();
        for (auto const& entry : tableData)
        {
            auto bcosEntry = toBcosEntry(entry.second);
            bcosTableData->emplace(entry.first, bcosEntry);
        }
        tableDatas.emplace_back(bcosTableData);
    }

    bcosTableFactory->importData(tableInfos, tableDatas, true);

    return bcosTableFactory;
}

inline bcostars::TableFactory toTarsTableFactory(
    const bcos::storage::TableFactoryInterface::Ptr& tableFactory)
{
    bcostars::TableFactory tarsTableFactory;

    tarsTableFactory.num = tableFactory->blockNumber();
    auto tableDatas = tableFactory->exportData(tableFactory->blockNumber());
    // set the tableInfo
    for (auto const& tableInfo : tableDatas.first)
    {
        bcostars::TableInfo tarsTableInfo;
        tarsTableInfo.name = tableInfo->name;
        tarsTableInfo.fields = tableInfo->fields;
        tarsTableInfo._key = tableInfo->key;

        tarsTableFactory.tableInfos.emplace_back(tarsTableInfo);
    }
    // set the tableData
    for (auto tableData : tableDatas.second)
    {
        std::map<std::string, bcostars::Entry> tarsTableData;
        if (!tableData)
        {
            continue;
        }
        for (auto tableDataItem : *tableData)
        {
            auto tarsEntry = toTarsEntry(tableDataItem.second);
            tarsTableData[tableDataItem.first] = tarsEntry;
        }
        tarsTableFactory.datas.emplace_back(tarsTableData);
    }
    return tarsTableFactory;
}

inline bcostars::Condition toTarsCondition(const bcos::storage::Condition::Ptr& condition)
{
    bcostars::Condition tarsCondition;
    if (!condition)
    {
        return tarsCondition;
    }
    tarsCondition.offset = condition->getLimit().first;
    tarsCondition.size = condition->getLimit().second;
    for (auto const& it : condition->m_conditions)
    {
        tarsCondition.conditions.push_back((short)(it.cmp));
    }

    return tarsCondition;
}

}  // namespace bcostars