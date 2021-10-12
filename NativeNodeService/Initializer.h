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
 * @brief Initializer for all the modules
 * @file Initializer.h
 * @author: yujiechen
 * @date 2021-06-11
 */
#pragma once
#include "DispatcherInitializer.h"
#include "libinitializer/Common.h"
#include "libinitializer/LedgerInitializer.h"
#include "libinitializer/NetworkInitializer.h"
#include "libinitializer/PBFTInitializer.h"
#include "libinitializer/ProtocolInitializer.h"
#include "libinitializer/RpcInitializer.h"
#include "libinitializer/StorageInitializer.h"
#include <bcos-framework/libutilities/BoostLogInitializer.h>
#include <memory>

namespace bcos
{
namespace initializer
{
class Initializer
{
public:
    using Ptr = std::shared_ptr<Initializer>;
    Initializer() = default;
    virtual ~Initializer() { stop(); }

    virtual void init(std::string const& _configFilePath, std::string const& _genesisFile);

    virtual void start();
    virtual void stop();

    // for fake transactions(TODO: remove this)
    bcos::tool::NodeConfig::Ptr nodeConfig() { return m_nodeConfig; }
    ProtocolInitializer::Ptr protocolInitializer() { return m_protocolInitializer; }
    PBFTInitializer::Ptr pbftInitializer() { return m_pbftInitializer; }
    LedgerInitializer::Ptr ledgerInitializer() { return m_ledgerInitializer; }

private:
    BoostLogInitializer::Ptr m_logInitializer;
    bcos::tool::NodeConfig::Ptr m_nodeConfig;
    ProtocolInitializer::Ptr m_protocolInitializer;
    NetworkInitializer::Ptr m_networkInitializer;
    StorageInitializer::Ptr m_storageInitializer;

    LedgerInitializer::Ptr m_ledgerInitializer;
    std::shared_ptr<DispatcherInitializer> m_dispatcherInitializer;
    PBFTInitializer::Ptr m_pbftInitializer;
    RpcInitializer::Ptr m_rpcInitializer;
};
}  // namespace initializer
}  // namespace bcos