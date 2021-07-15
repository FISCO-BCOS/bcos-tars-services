#include "FrontServiceServer.h"

using namespace bcostars;

std::once_flag FrontServiceServer::m_onceFlag;
bcos::front::FrontServiceInterface::Ptr FrontServiceServer::m_front;
bcos::BoostLogInitializer::Ptr FrontServiceServer::m_logInitializer;
bcos::crypto::KeyFactory::Ptr FrontServiceServer::m_keyFactory;