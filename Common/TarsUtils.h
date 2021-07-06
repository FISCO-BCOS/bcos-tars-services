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

inline void addAllConfig(Application *app) {
    app->addAppConfig("nodes.json");
    app->addAppConfig("node.pem");
    app->addAppConfig("ssl.key");
    app->addAppConfig("ssl.crt");
    app->addAppConfig("ca.crt");
    app->addAppConfig("config.genesis");
    app->addAppConfig("config.ini");
}

}  // namespace bcostars