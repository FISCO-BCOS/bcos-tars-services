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
 * @brief Common for libinitializer
 * @file Common.h
 * @author: yujiechen
 * @date 2021-06-10
 */
#pragma once
#include <bcos-framework/libtool/Exceptions.h>
#include <bcos-framework/libutilities/DataConvertUtility.h>
#include <bcos-framework/libutilities/Exceptions.h>
#include <bcos-framework/libutilities/FileUtility.h>
#include <bcos-framework/libutilities/Log.h>
#include <openssl/engine.h>
#include <openssl/rsa.h>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <memory>
#define INITIALIZER_LOG(LEVEL) BCOS_LOG(LEVEL) << "[INITIALIZER]"

namespace bcos
{
namespace initializer
{
inline std::shared_ptr<bytes> loadPrivateKey(std::string const& _keyPath)
{
    auto keyContent = readContents(boost::filesystem::path(_keyPath));
    if (keyContent->empty())
    {
        return nullptr;
    }
    std::shared_ptr<EC_KEY> ecKey;
    try
    {
        INITIALIZER_LOG(INFO) << LOG_BADGE("SecureInitializer") << LOG_DESC("loading privateKey");
        std::shared_ptr<BIO> bioMem(BIO_new(BIO_s_mem()), [&](BIO* p) { BIO_free(p); });
        BIO_write(bioMem.get(), keyContent->data(), keyContent->size());

        std::shared_ptr<EVP_PKEY> evpPKey(PEM_read_bio_PrivateKey(bioMem.get(), NULL, NULL, NULL),
            [](EVP_PKEY* p) { EVP_PKEY_free(p); });
        if (!evpPKey)
        {
            return nullptr;
        }
        ecKey.reset(EVP_PKEY_get1_EC_KEY(evpPKey.get()), [](EC_KEY* p) { EC_KEY_free(p); });
    }
    catch (bcos::Exception& e)
    {
        INITIALIZER_LOG(ERROR) << LOG_BADGE("SecureInitializer")
                               << LOG_DESC("parse privateKey failed")
                               << LOG_KV("EINFO", boost::diagnostic_information(e));
        BOOST_THROW_EXCEPTION(bcos::tool::InvalidConfig()
                              << errinfo_comment("SecureInitializer: parse privateKey failed"));
    }
    std::shared_ptr<const BIGNUM> ecPrivateKey(
        EC_KEY_get0_private_key(ecKey.get()), [](const BIGNUM*) {});

    std::shared_ptr<char> privateKeyData(
        BN_bn2hex(ecPrivateKey.get()), [](char* p) { OPENSSL_free(p); });
    std::string keyHex(privateKeyData.get());
    return fromHexString(keyHex);
}
}  // namespace initializer
}  // namespace bcos
