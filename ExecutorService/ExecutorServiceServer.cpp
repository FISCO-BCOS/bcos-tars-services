#include "ExecutorServiceServer.h"

using namespace bcostars;

std::once_flag ExecutorServiceServer::m_initFlag;
std::shared_ptr<bcos::executor::ExecutorInterface> ExecutorServiceServer::m_executor;
bcos::crypto::CryptoSuite::Ptr ExecutorServiceServer::m_cryptoSuite;