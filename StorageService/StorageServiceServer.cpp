#include "StorageServiceServer.h"
#include <bcos-framework/interfaces/storage/Common.h>
#include <bcos-framework/interfaces/storage/StorageInterface.h>

using namespace bcostars;

std::once_flag StorageServiceServer::m_storageFlag;
bcos::storage::StorageInterface::Ptr StorageServiceServer::m_storage;
bcos::BoostLogInitializer::Ptr StorageServiceServer::m_logInitializer;
bcos::crypto::CryptoSuite::Ptr StorageServiceServer::m_cryptoSuite;
std::atomic_bool StorageServiceServer::m_stopped = {false};