#pragma once

#include "bcos-executor/TransactionExecutor.h"

namespace bcos::initializer
{
class ExecutorInitializer
{
public:
    static bcos::executor::TransactionExecutor::Ptr build(txpool::TxPoolInterface::Ptr txpool,
        storage::TransactionalStorageInterface::Ptr storage,
        protocol::ExecutionMessageFactory::Ptr executionMessageFactory,
        bcos::crypto::Hash::Ptr hashImpl, bool isWasm)
    {
        return std::make_shared<executor::TransactionExecutor>(
            txpool, storage, executionMessageFactory, hashImpl, isWasm);
    }
};
}  // namespace bcos::initializer