#pragma once
#include <bcos-framework/interfaces/protocol/ServiceDesc.h>
#include <bcos-framework/libutilities/Log.h>
#include <tarscpp/servant/Application.h>
#include <string>

#define RPCSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[RPCSERVICE][INITIALIZER]"

#define GATEWAYSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[GATEWAYSERVICE][INITIALIZER]"

namespace bcostars
{
inline std::string getProxyDesc(const std::string& serviceName)
{
    std::string desc = ServerConfig::Application + "." + serviceName;
    return desc;
}

inline std::string getLogPath()
{
    return ServerConfig::LogPath + "/" + ServerConfig::Application + "/" + ServerConfig::ServerName;
}

inline void addAllConfig(Application* app)
{
    app->addAppConfig("nodes.json");
    app->addAppConfig("node.pem");
    app->addAppConfig("config.genesis");
    app->addAppConfig("config.ini");
    app->addAppConfig("ca.crt");
    app->addAppConfig("ssl.key");
    app->addAppConfig("ssl.crt");
    app->addAppConfig("sm_ca.crt");
    app->addAppConfig("sm_ssl.crt");
    app->addAppConfig("sm_enssl.crt");
    app->addAppConfig("sm_ssl.key");
    app->addAppConfig("sm_enssl.key");
}

}  // namespace bcostars