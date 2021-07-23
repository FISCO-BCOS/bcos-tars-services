#include "RpcServiceServer.h"

std::once_flag bcostars::RpcServiceServer::m_initFlag;
std::once_flag m_initFlag;
bcos::BoostLogInitializer::Ptr bcostars::RpcServiceServer::m_logInitializer;
bcos::rpc::RPCInterface::Ptr bcostars::RpcServiceServer::m_rpcInterface;
std::atomic_bool bcostars::RpcServiceServer::m_running = {false};