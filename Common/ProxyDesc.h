#pragma once
#include "servant/Application.h"
#include <string>

namespace bcostars
{
inline std::string getProxyDesc(const std::string& servantName)
{
    std::string desc =
        ServerConfig::Application + "." + ServerConfig::ServerName + "." + servantName;
    return desc;
}
}  // namespace bcostars