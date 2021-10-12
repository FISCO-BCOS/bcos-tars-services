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
 * @brief initializer for the storage
 * @file StorageInitializer.cpp
 * @author: yujiechen
 * @date 2021-06-11
 */
#include "StorageInitializer.h"
#include <bcos-storage/KVDBImpl.h>
#include <bcos-storage/RocksDBAdapterFactory.h>
#include <bcos-storage/Storage.h>

using namespace bcos;
using namespace bcos::initializer;
using namespace bcos::storage;
using namespace bcos::tool;

void StorageInitializer::init(NodeConfig::Ptr _nodeConfig)
{
    auto factory = std::make_shared<RocksDBAdapterFactory>(_nodeConfig->storagePath());
    auto ret =
        factory->createRocksDB(_nodeConfig->storageDBName(), RocksDBAdapter::TABLE_PREFIX_LENGTH);
    if (!ret.first)
    {
        BOOST_THROW_EXCEPTION(OpenDBFailed() << errinfo_comment("createRocksDB failed!"));
    }
    auto kvDB = std::make_shared<KVDBImpl>(ret.first);
    auto adapter =
        factory->createAdapter(_nodeConfig->stateDBName(), RocksDBAdapter::TABLE_PREFIX_LENGTH);
    auto storage = std::make_shared<StorageImpl>(adapter, kvDB);
    // storage->disableCache();
    storage->start();
    m_storage = storage;
}