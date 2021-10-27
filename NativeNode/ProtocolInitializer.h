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
 * @brief initializer for the protocol module
 * @file ProtocolInitializer.h
 * @author: yujiechen
 * @date 2021-06-10
 */
#pragma once
#include "Common.h"
#include <bcos-framework/interfaces/crypto/CryptoSuite.h>
#include <bcos-framework/interfaces/crypto/KeyFactory.h>
#include <bcos-framework/interfaces/protocol/BlockFactory.h>
#include <bcos-framework/interfaces/protocol/TransactionSubmitResultFactory.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <libprotocol/TransactionSubmitResultFactoryImpl.h>

namespace bcos
{
namespace initializer
{
class ProtocolInitializer
{
public:
    using Ptr = std::shared_ptr<ProtocolInitializer>;
    ProtocolInitializer() = default;
    virtual ~ProtocolInitializer() {}

    virtual void init(bcos::tool::NodeConfig::Ptr _nodeConfig);

    bcos::crypto::CryptoSuite::Ptr cryptoSuite() { return m_cryptoSuite; }
    bcos::protocol::BlockFactory::Ptr blockFactory() { return m_blockFactory; }
    bcos::protocol::TransactionSubmitResultFactory::Ptr txResultFactory()
    {
        return m_txResultFactory;
    }

    bcos::crypto::KeyPairInterface::Ptr keyPair() const { return m_keyPair; }
    bcos::protocol::TransactionFactory::Ptr transactionFactory() const
    {
        return m_transactionFactory;
    }

private:
    void createCryptoSuite();
    void createSMCryptoSuite();
    void loadKeyPair(bcos::tool::NodeConfig::Ptr _nodeConfig);
    void createFactory();

private:
    bcos::crypto::CryptoSuite::Ptr m_cryptoSuite;
    bcos::protocol::BlockFactory::Ptr m_blockFactory;
    bcos::protocol::TransactionSubmitResultFactory::Ptr m_txResultFactory;
    bcos::crypto::KeyPairInterface::Ptr m_keyPair;
    bcos::protocol::TransactionFactory::Ptr m_transactionFactory;
};
}  // namespace initializer
}  // namespace bcos