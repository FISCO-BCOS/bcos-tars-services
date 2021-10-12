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
 * @file Initializer.cpp
 * @author: yujiechen
 * @date 2021-06-11
 */
#include "Initializer.h"
#include "LedgerInitializer.h"
#include "SchedulerInitializer.h"
#include "StorageInitializer.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-gateway/Gateway.h>

using namespace bcos;
using namespace bcos::tool;
using namespace bcos::initializer;

void Initializer::init(std::string const& _configFilePath, std::string const& _genesisFile)
{
    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::read_ini(_configFilePath, pt);
        m_logInitializer = std::make_shared<BoostLogInitializer>();
        m_logInitializer->initLog(pt);

        // loadConfig
        m_nodeConfig =
            std::make_shared<NodeConfig>(std::make_shared<bcos::crypto::KeyFactoryImpl>());
        m_nodeConfig->loadConfig(_configFilePath);
        m_nodeConfig->loadGenesisConfig(_genesisFile);

        // init the protocol
        m_protocolInitializer = std::make_shared<ProtocolInitializer>();
        m_protocolInitializer->init(m_nodeConfig);

        std::string nodeID = m_protocolInitializer->keyPair()->publicKey()->hex();
        // init the network
        m_networkInitializer = std::make_shared<NetworkInitializer>();
        m_networkInitializer->init(
            _configFilePath, m_nodeConfig, m_protocolInitializer->keyPair()->publicKey());

        auto storage = StorageInitializer::build(m_nodeConfig);

        auto ledger =
            LedgerInitializer::build(m_protocolInitializer->blockFactory(), storage, m_nodeConfig);

        auto scheduler = SchedulerInitializer::build(m_protocolInitializer, ledger, storage);

        // init the pbft related modules
        m_pbftInitializer = std::make_shared<PBFTInitializer>();
        m_pbftInitializer->init(
            m_nodeConfig, m_protocolInitializer, m_networkInitializer, ledger, scheduler, storage);

        m_rpcInitializer = std::make_shared<RpcInitializer>();
        m_rpcInitializer->setNodeID(nodeID);
        m_rpcInitializer->setNetworkInitializer(m_networkInitializer);
        m_rpcInitializer->setNodeConfig(m_nodeConfig);
        m_rpcInitializer->setFrontService(m_networkInitializer->frontService());
        m_rpcInitializer->setLedger(ledger);
        m_rpcInitializer->setTxPoolInterface(m_pbftInitializer->txpool());
        m_rpcInitializer->setExecutorInterface(m_dispatcherInitializer->executor());
        m_rpcInitializer->setConsensusInterface(m_pbftInitializer->pbft());
        m_rpcInitializer->setBlockSyncInterface(m_pbftInitializer->blockSync());
        m_rpcInitializer->setGatewayInterface(m_networkInitializer->gateway());
        m_rpcInitializer->setTransactionFactory(m_protocolInitializer->transactionFactory());
        m_rpcInitializer->init(m_nodeConfig, _configFilePath);

        dispatcher->init(m_pbftInitializer->txpool());
    }
    catch (std::exception const& e)
    {
        std::cout << "init bcos-node failed for " << boost::diagnostic_information(e);
        exit(-1);
    }
}

void Initializer::start()
{
    try
    {
        if (m_dispatcherInitializer)
        {
            m_dispatcherInitializer->start();
        }

        if (m_pbftInitializer)
        {
            m_pbftInitializer->start();
        }

        if (m_networkInitializer)
        {
            m_networkInitializer->start();
        }

        if (m_rpcInitializer)
        {
            m_rpcInitializer->start();
        }
    }
    catch (std::exception const& e)
    {
        std::cout << "start bcos-node failed for " << boost::diagnostic_information(e);
        exit(-1);
    }
}

void Initializer::stop()
{
    try
    {
        if (m_rpcInitializer)
        {
            m_rpcInitializer->stop();
        }
        if (m_networkInitializer)
        {
            m_networkInitializer->stop();
        }
        if (m_pbftInitializer)
        {
            m_pbftInitializer->stop();
        }
        if (m_dispatcherInitializer)
        {
            m_dispatcherInitializer->stop();
        }
        if (m_storageInitializer)
        {
            m_storageInitializer->stop();
        }
    }
    catch (std::exception const& e)
    {
        std::cout << "stop bcos-node failed for " << boost::diagnostic_information(e);
        exit(-1);
    }
}