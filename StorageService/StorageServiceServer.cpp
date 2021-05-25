#include "StorageServiceServer.h"
#include <bcos-framework/interfaces/storage/Common.h>
#include <bcos-framework/interfaces/storage/StorageInterface.h>

using namespace bcostars;

std::mutex StorageServiceServer::m_initLock;
bcos::storage::StorageInterface::Ptr StorageServiceServer::m_storage;