/**
 *  Copyright (C) 2021 FISCO BCOS.
 *  SPDX-License-Identifier: Apache-2.0
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 * @brief initializer for the dispatcher and executor
 * @file DispatcherInitializer.h
 * @author: yujiechen
 * @date 2021-06-21
 */
#pragma once
#include "libinitializer/Common.h"
#include "libinitializer/ProtocolInitializer.h"
#include <bcos-dispatcher/DispatcherImpl.h>
#include <bcos-framework/interfaces/dispatcher/DispatcherInterface.h>
#include <bcos-framework/interfaces/executor/ExecutorInterface.h>
#include <bcos-framework/interfaces/ledger/LedgerInterface.h>
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <bcos-framework/libtool/NodeConfig.h>

namespace bcos
{
namespace initializer
{
class DispatcherInitializer
{
public:
    DispatcherInitializer() = default;
    virtual ~DispatcherInitializer() {}

    virtual void init(bcos::tool::NodeConfig::Ptr _nodeConfig,
        ProtocolInitializer::Ptr _protocolInitializer, bcos::ledger::LedgerInterface::Ptr _ledger,
        bcos::storage::StorageInterface::Ptr _stateStorage);

    bcos::dispatcher::DispatcherImpl::Ptr dispatcher() { return m_dispatcher; }
    std::shared_ptr<bcos::executor::ExecutorInterface> executor() { return m_executor;} 

    virtual void start();
    virtual void stop();

private:
    bcos::dispatcher::DispatcherImpl::Ptr m_dispatcher;
    std::shared_ptr<bcos::executor::ExecutorInterface> m_executor;
};
}  // namespace initializer
}  // namespace bcos