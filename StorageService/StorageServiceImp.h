#pragma once

#include "servant/Application.h"
#include "StorageService.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>

namespace bcostars {

class StorageServiceImp : public StorageService
{
public:
    virtual ~StorageServiceImp() {}

    virtual void initialize();

    virtual void destroy();

    virtual int test(tars::TarsCurrentPtr current);

    private:
    bcos::storage::StorageInterface::Ptr m_storage;
};

}