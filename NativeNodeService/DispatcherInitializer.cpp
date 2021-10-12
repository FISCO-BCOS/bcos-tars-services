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
 */
#include "DispatcherInitializer.h"
#include <bcos-executor/Executor.h>

using namespace bcos;
using namespace bcos::tool;
using namespace bcos::initializer;
using namespace bcos::dispatcher;
using namespace bcos::executor;
using namespace bcos::ledger;
using namespace bcos::storage;

void DispatcherInitializer::init(NodeConfig::Ptr _nodeConfig,
    ProtocolInitializer::Ptr _protocolInitializer, LedgerInterface::Ptr _ledger,
    StorageInterface::Ptr _stateStorage)
{
    m_dispatcher = std::make_shared<DispatcherImpl>();
    m_executor = std::make_shared<Executor>(_protocolInitializer->blockFactory(), m_dispatcher,
        _ledger, _stateStorage, _nodeConfig->isWasm());
}

void DispatcherInitializer::start()
{
    m_dispatcher->start();
    m_executor->start();
}

void DispatcherInitializer::stop()
{
    m_dispatcher->stop();
    m_executor->stop();
}