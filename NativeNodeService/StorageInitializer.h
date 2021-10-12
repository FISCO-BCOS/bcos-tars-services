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
 * @file StorageInitializer.h
 * @author: yujiechen
 * @date 2021-06-11
 */
#pragma once
#include "libinitializer/Common.h"
#include <bcos-framework/interfaces/storage/StorageInterface.h>
#include <bcos-framework/libtool/NodeConfig.h>
namespace bcos
{
namespace initializer
{
class StorageInitializer
{
public:
    using Ptr = std::shared_ptr<StorageInitializer>;
    StorageInitializer() = default;
    virtual ~StorageInitializer() {}

    void stop()
    {
        if (m_storage)
        {
            m_storage->stop();
        }
    }

    virtual void init(bcos::tool::NodeConfig::Ptr _nodeConfig);
    bcos::storage::StorageInterface::Ptr storage() { return m_storage; }

private:
    bcos::storage::StorageInterface::Ptr m_storage;
};
}  // namespace initializer
}  // namespace bcos