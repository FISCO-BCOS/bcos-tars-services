#pragma once

#include "servant/Application.h"
#include "Storage.h"

namespace bcos_tars {

class StorageImp : public Storage
{
public:
    virtual ~StorageImp() {}

    virtual void initialize();

    virtual void destroy();

    virtual int test(tars::TarsCurrentPtr current);
};
/////////////////////////////////////////////////////
}