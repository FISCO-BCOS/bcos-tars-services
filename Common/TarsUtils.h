#pragma once
#include "servant/Application.h"
#include <string>

namespace bcostars
{
const std::string STORAGE_SERVANT_NAME = "StorageServiceObj";
const std::string STORAGE_SERVICE_NAME = "StorageService." + STORAGE_SERVANT_NAME;

const std::string DISPATCHER_SERVANT_NAME = "DispatcherServiceObj";
const std::string DISPATCHER_SERVICE_NAME = "DispatcherService." + DISPATCHER_SERVANT_NAME;

const std::string FRONT_SERVANT_NAME = "FrontServiceObj";
const std::string FRONT_SERVICE_NAME = "FrontService." + FRONT_SERVANT_NAME;

const std::string GATEWAY_SERVANT_NAME = "GatewayServiceObj";
const std::string GATEWAY_SERVICE_NAME = "GatewayService." + GATEWAY_SERVANT_NAME;

const std::string EXECUTOR_SERVANT_NAME = "ExecutorServiceObj";
const std::string EXECUTOR_SERVICE_NAME = "ExecutorService." + EXECUTOR_SERVANT_NAME;

const std::string TXPOOL_SERVANT_NAME = "TxPoolServiceObj";
const std::string TXPOOL_SERVICE_NAME = "TxPoolService." + TXPOOL_SERVANT_NAME;

const std::string PBFT_SERVANT_NAME = "PBFTServiceObj";
const std::string PBFT_SERVICE_NAME = "PBFTService." + PBFT_SERVANT_NAME;

const std::string RPC_SERVANT_NAME = "RpcServiceObj";
const std::string RPC_SERVICE_NAME = "RpcService." + RPC_SERVANT_NAME;

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
    app->addAppConfig("ssl.key");
    app->addAppConfig("ssl.crt");
    app->addAppConfig("ca.crt");
    app->addAppConfig("config.genesis");
    app->addAppConfig("config.ini");
}

}  // namespace bcostars