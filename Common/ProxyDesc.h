#pragma once
#include <string>
#include "servant/Application.h"

namespace bcostars {
    inline std::string getProxyDesc(const std::string &servantName) {
        std::string desc = ServerConfig::Application + "." + ServerConfig::ServerName + "." + servantName;
        return desc;
    }
}