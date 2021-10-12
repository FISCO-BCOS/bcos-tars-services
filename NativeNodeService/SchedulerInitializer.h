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
#include "Common.h"
#include "ProtocolInitializer.h"
#include "interfaces/dispatcher/SchedulerInterface.h"
#include <bcos-framework/interfaces/dispatcher/DispatcherInterface.h>
#include <bcos-framework/interfaces/executor/ExecutorInterface.h>
#include <bcos-framework/interfaces/ledger/LedgerInterface.h>
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <bcos-framework/libexecutor/NativeExecutionMessage.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-scheduler/SchedulerImpl.h>

namespace bcos::initializer
{
class SchedulerInitializer
{
public:
    static bcos::scheduler::SchedulerInterface::Ptr build(
        ProtocolInitializer::Ptr _protocolInitializer, bcos::ledger::LedgerInterface::Ptr _ledger,
        bcos::storage::StorageInterface::Ptr storage)
    {
        auto executionMessageFactory =
            std::make_shared<bcos::executor::NativeExecutionMessageFactory>();

        return std::make_shared<scheduler::SchedulerImpl>(_ledger, storage, executionMessageFactory,
            nullptr, nullptr, _protocolInitializer->cryptoSuite()->hashImpl());
    }
};
}  // namespace bcos::initializer