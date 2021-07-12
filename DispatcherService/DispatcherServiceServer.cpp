#include "DispatcherServiceServer.h"

using namespace bcostars;

std::once_flag DispatcherServiceServer::m_initFlag;
bcos::dispatcher::DispatcherImpl::Ptr DispatcherServiceServer::m_dispatcher;
bcostars::protocol::BlockFactoryImpl::Ptr DispatcherServiceServer::m_blockFactory;
bcos::BoostLogInitializer::Ptr DispatcherServiceServer::m_logInitializer;
std::atomic_bool DispatcherServiceServer::m_running = {false};