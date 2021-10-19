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
#include "ExecutorInitializer.h"
#include "LedgerInitializer.h"
#include "SchedulerInitializer.h"
#include "StorageInitializer.h"
#include "libexecutor/NativeExecutionMessage.h"
#include <bcos-crypto/signature/key/KeyFactoryImpl.h>
#include <bcos-framework/libtool/NodeConfig.h>
#include <bcos-scheduler/ExecutorManager.h>
#include <bcos-tars-protocol/client/GatewayServiceClient.h>

using namespace bcos;
using namespace bcos::tool;
using namespace bcos::initializer;

void Initializer::init(std::string const& _configFilePath, std::string const& _genesisFile,
    std::string const& _privateKeyPath)
{
    try
    {
        // loadConfig
        m_nodeConfig =
            std::make_shared<NodeConfig>(std::make_shared<bcos::crypto::KeyFactoryImpl>());
        m_nodeConfig->loadConfig(_configFilePath);
        m_nodeConfig->loadGenesisConfig(_genesisFile);

        // init the protocol
        m_protocolInitializer = std::make_shared<ProtocolInitializer>();
        m_protocolInitializer->init(m_nodeConfig);
        m_protocolInitializer->loadKeyPair(_privateKeyPath);

        // get gateway client
        auto gatewayPrx =
            Application::getCommunicator()->stringToProxy<bcostars::GatewayServicePrx>(
                m_nodeConfig->gatewayServiceName());
        auto gateWay = std::make_shared<bcostars::GatewayServiceClient>(
            gatewayPrx, m_protocolInitializer->cryptoSuite()->keyFactory());

        // build the front service
        m_frontServiceInitializer =
            std::make_shared<FrontServiceInitializer>(m_nodeConfig, m_protocolInitializer, gateWay);

        // build the storage
        auto storagePath = ServerConfig::BasePath + "../" + m_nodeConfig->groupId() + "/" +
                           m_nodeConfig->storagePath();
        auto storage = StorageInitializer::build(storagePath);

        // build ledger
        auto ledger =
            LedgerInitializer::build(m_protocolInitializer->blockFactory(), storage, m_nodeConfig);
        m_ledger = ledger;

        auto executionMessageFactory = std::make_shared<executor::NativeExecutionMessageFactory>();
        auto executorManager = std::make_shared<bcos::scheduler::ExecutorManager>();

        m_scheduler = SchedulerInitializer::build(executorManager, ledger, storage,
            executionMessageFactory, m_protocolInitializer->blockFactory(),
            m_protocolInitializer->cryptoSuite()->hashImpl());

        // init the txpool
        m_txpoolInitializer = std::make_shared<TxPoolInitializer>(
            m_nodeConfig, m_protocolInitializer, m_frontServiceInitializer->front(), ledger);

        // build and init the pbft related modules
        m_pbftInitializer =
            std::make_shared<PBFTInitializer>(ServerConfig::Application, _genesisFile,
                _configFilePath, m_nodeConfig, m_protocolInitializer, m_txpoolInitializer->txpool(),
                ledger, m_scheduler, storage, m_frontServiceInitializer->front());
        m_pbftInitializer->init();

        // init the txpool
        m_txpoolInitializer->init(m_pbftInitializer->sealer());

        // init the frontService
        m_frontServiceInitializer->init(m_pbftInitializer->pbft(), m_pbftInitializer->blockSync(),
            m_txpoolInitializer->txpool());

        auto executor = ExecutorInitializer::build(m_txpoolInitializer->txpool(), storage,
            executionMessageFactory, m_protocolInitializer->cryptoSuite()->hashImpl(), false);
        executorManager->addExecutor("default", executor);
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
        if (m_txpoolInitializer)
        {
            m_txpoolInitializer->start();
        }
        if (m_pbftInitializer)
        {
            m_pbftInitializer->start();
        }

        if (m_frontServiceInitializer)
        {
            m_frontServiceInitializer->start();
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
        if (m_frontServiceInitializer)
        {
            m_frontServiceInitializer->stop();
        }
        if (m_pbftInitializer)
        {
            m_pbftInitializer->stop();
        }
        if (m_txpoolInitializer)
        {
            m_txpoolInitializer->stop();
        }
    }
    catch (std::exception const& e)
    {
        std::cout << "stop bcos-node failed for " << boost::diagnostic_information(e);
        exit(-1);
    }
}