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
 * @brief main for the fisco-bcos
 * @file main.cpp
 * @author: yujiechen
 * @date 2021-07-26
 */
#include "fisco-bcos/utilities/Common.h"
#include "libinitializer/Initializer.h"
#include <chrono>
#include <ctime>
using namespace bcos;
using namespace bcos::initializer;
using namespace bcos::node;

int main(int argc, const char* argv[])
{
    /// set LC_ALL
    setDefaultOrCLocale();
    std::set_terminate([]() {
        std::cerr << "terminate handler called" << std::endl;
        abort();
    });
    // get datetime and output welcome info
    ExitHandler exitHandler;
    signal(SIGTERM, &ExitHandler::exitHandler);
    signal(SIGABRT, &ExitHandler::exitHandler);
    signal(SIGINT, &ExitHandler::exitHandler);
    // Note: the initializer must exist in the life time of the whole program
    auto initializer = std::make_shared<Initializer>();
    try
    {
        auto param = initCommandLine(argc, argv, false);
        initializer->init(param.configFilePath, param.genesisFilePath);
        initializer->start();
    }
    catch (std::exception const& e)
    {
        std::cerr << "Init failed!!!" << std::endl;
        return -1;
    }
    printVersion();
    std::cout << "[" << getCurrentDateTime() << "] ";
    std::cout << "The fisco-bcos is running..." << std::endl;
    while (!exitHandler.shouldExit())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    initializer.reset();
    std::cout << "[" << getCurrentDateTime() << "] ";
    std::cout << "fisco-bcos program exit normally." << std::endl;
}