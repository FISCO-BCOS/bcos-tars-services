#include "TxPoolServiceServer.h"

std::once_flag bcostars::TxPoolServiceServer::m_initFlag;
std::once_flag m_initFlag;
bcos::txpool::TxPool::Ptr bcostars::TxPoolServiceServer::m_txpool;
std::shared_ptr<bcos::ledger::Ledger> bcostars::TxPoolServiceServer::m_ledger;
bcos::crypto::CryptoSuite::Ptr bcostars::TxPoolServiceServer::m_cryptoSuite;
bcos::BoostLogInitializer::Ptr bcostars::TxPoolServiceServer::m_logInitializer;
std::atomic_bool bcostars::TxPoolServiceServer::m_running = {false};