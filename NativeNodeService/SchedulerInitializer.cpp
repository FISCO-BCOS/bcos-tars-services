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
 * @file DispatcherInitializer.cpp
 * @author: yujiechen
 * @date 2021-06-21
 * @brief initializer for the dispatcher and executor
 * @file SchedulerInitializer.cpp
 * @author: ancelmo
 * @date 2021-10-14
 */
#include "SchedulerInitializer.h"
#include "bcos-framework/libexecutor/NativeExecutionMessage.h"

using namespace bcos;
using namespace bcos::initializer;

void SchedulerInitializer::init(ProtocolInitializer::Ptr _protocolInitializer,
    bcos::ledger::LedgerInterface::Ptr _ledger, bcos::storage::StorageInterface::Ptr storage)
{
    auto executionMessageFactory =
        std::make_shared<bcos::executor::NativeExecutionMessageFactory>();

    m_scheduler = std::make_shared<scheduler::SchedulerImpl>(_ledger, storage,
        executionMessageFactory, nullptr, nullptr, _protocolInitializer->cryptoSuite()->hashImpl());
}