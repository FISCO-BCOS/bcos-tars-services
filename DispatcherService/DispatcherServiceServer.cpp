#include "DispatcherServiceServer.h"

using namespace bcostars;

std::once_flag DispatcherServiceServer::m_initFlag;
bcos::dispatcher::DispatcherImpl::Ptr DispatcherServiceServer::m_dispatcher;
bcostars::protocol::BlockHeaderFactoryImpl::Ptr DispatcherServiceServer::m_blockHeaderFactory;
bcostars::protocol::BlockFactoryImpl::Ptr DispatcherServiceServer::m_blockFactory;