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
 * @brief Application for the NativeNode
 * @file NativeNodeApp.cpp
 * @author: yujiechen
 * @date 2021-10-18
 */
#include "NativeNodeApp.h"
using namespace bcostars;
using namespace bcos;
using namespace bcos::initializer;
using namespace bcos::protocol;

void NativeNodeApp::initialize()
{
    initConfig();
    initLog();
    initNodeService();
    initTarsNodeService();
}

void NativeNodeApp::initLog()
{
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(m_iniConfigPath, pt);
    m_logInitializer = std::make_shared<BoostLogInitializer>();
    m_logInitializer->setLogPath(getLogPath());
    m_logInitializer->initLog(pt);
}

void NativeNodeApp::initNodeService()
{
    m_nodeInitializer = std::make_shared<Initializer>();
    m_nodeInitializer->init(m_iniConfigPath, m_genesisConfigPath, m_privateKeyPath);
    m_nodeInitializer->start();
}

void NativeNodeApp::initTarsNodeService()
{
    // init the txpool servant
    TxPoolServiceParam txpoolParam;
    txpoolParam.txPoolInitializer = m_nodeInitializer->txPoolInitializer();
    addServantWithParams<TxPoolServiceServer, TxPoolServiceParam>(
        getProxyDesc(TXPOOL_SERVANT_NAME), txpoolParam);

    // init the pbft servant
    PBFTServiceParam pbftParam;
    pbftParam.pbftInitializer = m_nodeInitializer->pbftInitializer();
    addServantWithParams<PBFTServiceServer, PBFTServiceParam>(
        getProxyDesc(CONSENSUS_SERVANT_NAME), pbftParam);

    // init the ledger
    LedgerServiceParam ledgerParam;
    ledgerParam.ledger = m_nodeInitializer->ledger();
    addServantWithParams<LedgerServiceServer, LedgerServiceParam>(
        getProxyDesc(LEDGER_SERVANT_NAME), ledgerParam);

    // init the scheduler
    SchedulerServiceParam schedulerParam;
    schedulerParam.scheduler = m_nodeInitializer->scheduler();
    schedulerParam.cryptoSuite = m_nodeInitializer->protocolInitializer()->cryptoSuite();
    addServantWithParams<SchedulerServiceServer, SchedulerServiceParam>(
        getProxyDesc(SCHEDULER_SERVANT_NAME), schedulerParam);

    // init the frontService, for the gateway to access the frontService
    FrontServiceParam frontServiceParam;
    frontServiceParam.frontServiceInitializer = m_nodeInitializer->frontService();
    addServantWithParams<FrontServiceServer, FrontServiceParam>(
        getProxyDesc(FRONT_SERVANT_NAME), frontServiceParam);
}
