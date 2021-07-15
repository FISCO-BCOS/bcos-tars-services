#include "GatewayServiceServer.h"

using namespace bcostars;

std::once_flag GatewayServiceServer::m_initFlag;
bcos::gateway::Gateway::Ptr GatewayServiceServer::m_gateway;
bcos::BoostLogInitializer::Ptr GatewayServiceServer::m_logInitializer;
bcos::crypto::KeyFactory::Ptr GatewayServiceServer::m_keyFactory;