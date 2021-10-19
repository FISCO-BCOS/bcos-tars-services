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
 * @brief server for the NativeNode
 * @file NativeNode.h
 * @author: yujiechen
 * @date 2021-10-18
 */
#pragma once
#include "../LedgerService/LedgerServiceServer.h"
#include "../PBFTService/PBFTServiceServer.h"
#include "../SchedulerService/SchedulerServiceServer.h"
#include "../TxPoolService/TxPoolServiceServer.h"

namespace bcostars
{
struct NativeNodeParam
{
    TxPoolServiceParam txpoolParam;
    PBFTServiceParam pbftParam;
    LedgerServiceParam ledgerParam;
    SchedulerServiceParam schedulerParam;
};
class NativeNode : public TxPoolServiceServer,
                   public PBFTServiceServer,
                   public LedgerServiceServer,
                   public SchedulerServiceServer
{
public:
    NativeNode(NativeNodeParam const& _param)
      : TxPoolServiceServer(_param.txpoolParam),
        PBFTServiceServer(_param.pbftParam),
        LedgerServiceServer(_param.ledgerParam),
        SchedulerServiceServer(_param.schedulerParam)
    {}
    ~NativeNode() override {}

    void initialize() override {}
    void destroy() override {}

    void setName(const string& _name) override { TxPoolServiceServer::setName(_name); }
    void setApplication(Application* _application) override
    {
        TxPoolServiceServer::setApplication(_application);
    }
};
}  // namespace bcostars