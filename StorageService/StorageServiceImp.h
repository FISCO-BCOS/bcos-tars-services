#pragma once

#include "servant/Application.h"
#include "StorageService.h"

namespace bcostars {

class StorageServiceImp : public StorageService
{
public:
    virtual ~StorageServiceImp() {}

    virtual void initialize();

    virtual void destroy();

    virtual int test(tars::TarsCurrentPtr current);
};
/////////////////////////////////////////////////////
}