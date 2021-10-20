#pragma once
#include <bcos-framework/interfaces/protocol/ServiceDesc.h>
#include <bcos-framework/libutilities/Log.h>
#include <tarscpp/servant/Application.h>
#include <string>

#define RPCSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[RPCSERVICE][INITIALIZER]"
#define GATEWAYSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[GATEWAYSERVICE][INITIALIZER]"
#define TXPOOLSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[TXPOOLSERVICE]"
#define PBFTSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[PBFTSERVICE]"
#define FRONTSERVICE_LOG(LEVEL) BCOS_LOG(LEVEL) << "[FRONTSERVICE]"

namespace bcostars
{
inline std::string getProxyDesc(const std::string& serviceName, std::string const& _servantName)
{
    std::string desc = ServerConfig::Application + "." + serviceName + "." + _servantName;
    return desc;
}

inline std::string getLogPath()
{
    return ServerConfig::LogPath + "/" + ServerConfig::Application + "/" + ServerConfig::ServerName;
}
}  // namespace bcostars