#include "FrontServiceServer.h"

using namespace bcostars;

std::once_flag FrontServiceServer::m_onceFlag;
bcos::front::FrontServiceInterface::Ptr FrontServiceServer::m_front;