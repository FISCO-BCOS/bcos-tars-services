#pragma once

#include "servant/Application.h"
#include "StorageService.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>


namespace bcostars {

class StorageServiceServer : public StorageService
{
public:
    ~StorageServiceServer() override {}

    void initialize() override;

    void destroy() override;

    tars::Int32 getPrimaryKeys(const bcostars::TableInfo & tableInfo,const bcostars::Condition & condition,vector<std::string> &keys,tars::TarsCurrentPtr current) override;

    private:
    bcos::storage::StorageInterface::Ptr m_storage;
};

}