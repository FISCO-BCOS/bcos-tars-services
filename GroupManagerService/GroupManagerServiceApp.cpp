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
 * @brief application for GroupManagerService
 * @file GroupManagerServiceApp.cpp
 * @author: yujiechen
 * @date 2021-10-18
 */
#include "GroupManagerServiceApp.h"
#include "boost/filesystem.hpp"
#include <bcos-storage/RocksDBStorage.h>
#include <rocksdb/write_batch.h>
#include <tarscpp/framework/AdminReg.h>

using namespace bcostars;
using namespace bcos;
using namespace bcos::group;

void GroupManagerServiceApp::initService()
{
    // init the log
    boost::property_tree::ptree pt;
    boost::property_tree::read_ini(m_iniConfigPath, pt);
    m_logInitializer = std::make_shared<BoostLogInitializer>();
    m_logInitializer->setLogPath(getLogPath());
    m_logInitializer->initLog(pt);

    m_groupInfoFactory = std::make_shared<GroupInfoFactory>();
    m_chainNodeInfoFactory = std::make_shared<ChainNodeInfoFactory>();

    // get AdminRegProxy client
    auto const& config = getConfig();
    auto adminPrx = Application::getCommunicator()->stringToProxy<AdminRegPrx>(
        config.get("/tars/objname<AdminRegObjName>", ""));

    // create storage
    auto configParser = std::make_shared<ConfigParser>(m_iniConfigPath);
    auto storagePath = ServerConfig::BasePath + "../storage/" + configParser->storagePath();
    GROUP_LOG(INFO) << LOG_DESC("create storage") << LOG_KV("storagePath", storagePath);
    auto storage = createStorage(storagePath);

    auto factory = std::make_shared<GroupManagerFactory>();
    m_groupManager =
        factory->build(adminPrx, configParser, m_groupInfoFactory, m_chainNodeInfoFactory, storage);
    GROUP_LOG(INFO) << LOG_DESC("int groupManager success");
}

bcos::storage::StorageInterface::Ptr GroupManagerServiceApp::createStorage(
    std::string const& _storagePath)
{
    boost::filesystem::create_directories(_storagePath);
    // create storage
    rocksdb::DB* db;
    rocksdb::Options options;

    // Optimize RocksDB. This is the easiest way to get RocksDB to perform well
    options.IncreaseParallelism();
    options.OptimizeLevelStyleCompaction();
    // create the DB if it's not already present
    options.create_if_missing = true;
    // open DB
    rocksdb::Status s = rocksdb::DB::Open(options, _storagePath, &db);
    if (!s.ok())
    {
        throw std::runtime_error("createRocksDB failed!");
    }
    return std::make_shared<bcos::storage::RocksDBStorage>(std::unique_ptr<rocksdb::DB>(db));
}