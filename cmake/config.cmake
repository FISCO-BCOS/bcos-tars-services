# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
    URL https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/a539aa22dfe58e429f5731c9a4b863ff74ab1777.tar.gz
    SHA1 768604ad88b0422de456262c95cd1a5efa2e9b44
	CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON #DEBUG=ON
)

hunter_config(bcos-crypto
    VERSION 3.0.0-local-43df7523
    URL https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/25c8edb7d5cbadb514bbce9733573c8ffdb3600d.tar.gz
    SHA1 4a1649e7095f5db58a5ae0671b2278bcccc25f1d
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/09e8d258f91e8ba6cad9fa63b7d8bd6ef1700f76.tar.gz"
    SHA1 e5b052900a0ccb50e31abc839982da06640cdb81
)

hunter_config(bcos-pbft VERSION 3.0.0-local-a2a9f7d2
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/996871e97fffb06e8dd16e55c0cdc539a6f341a5.tar.gz"
    SHA1 1a6546d213fcb27efbe16e02e2b690e1198c638f
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/9ae4fc3f65096deedb064f9fbf7a0ff6b5d1e599.tar.gz"
    SHA1 d1add6f4f4f718a277a9188f97fa98e30537d4ab
)

hunter_config(rocksdb VERSION 6.20.3
	URL https://${URL_BASE}/facebook/rocksdb/archive/refs/tags/v6.20.3.tar.gz
    SHA1 64e4e6031820026c051d6e2072c0197e3bce1643
    CMAKE_ARGS WITH_TESTS=OFF WITH_GFLAGS=OFF WITH_BENCHMARK_TOOLS=OFF WITH_CORE_TOOLS=OFF
    WITH_TOOLS=OFF PORTABLE=ON FAIL_ON_WARNINGS=OFF WITH_ZSTD=ON BUILD_SHARED_LIBS=OFF
)

hunter_config(bcos-storage VERSION 3.0.0-local-900836da
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/900836da481856ca13a1184943bb0d18d2124a88.tar.gz"
    SHA1 7053cf63e832367d561693e9d438e9048c087091
    CMAKE_ARGS URL_BASE=${URL_BASE} #CMAKE_BUILD_TYPE=Debug
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/f5f92e4fb48f75fd808d40366629996a7d37fa19.tar.gz"
    SHA1 3aea1cf27c585cb7cc58e4c2e2c9fe75d7d44fb6
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/15e18804aab90def4c2ba7d811024df921f935de.tar.gz"
    SHA1 1485c64a31b106f912aa1d1878da5d91dc0a2975
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/9467ea52f13e8fbb11c634d857b8107a1adb387a.tar.gz"
    SHA1 e5c22b3231d9adbb41a62fee8dbbab0cc83282de
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-dispatcher VERSION 3.0.0-local-2903b298
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-dispatcher/archive/637b83d4977ec9490cfbaf90795aa8b100041a36.tar.gz"
    SHA1 a8e18380f300092403f6139bf9c6703a1ece3333
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-rpc VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-rpc/archive/122e317cc074cc0636b3da25996b38c584bab1a9.tar.gz"
    SHA1 71a9e7f78306bc1097cdf1ed41b754078275cf4b
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

# hunter_config(bcos-executor VERSION 3.0.0-local-ac6d5d18
#     URL "https://${URL_BASE}/FISCO-BCOS/bcos-executor/archive/8a2dc4e32fc4de6a9a95ed2fbc83540e7ce589b4.tar.gz"
#     SHA1 c8254cd8a943ea21ab2bdbacd5e54d50ad20f778
#     CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON DEBUG=ON
# )

hunter_config(evmc VERSION 7.3.0-d951b1ef
    URL https://${URL_BASE}/FISCO-BCOS/evmc/archive/d951b1ef088be6922d80f41c3c83c0cbd69d2bfa.tar.gz
    SHA1 0b39b36cd8533c89ee0b15b6e94cff1111383ac7
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(intx VERSION 0.4.1 URL https://github.com/chfast/intx/archive/v0.4.0.tar.gz
    SHA1 8a2a0b0efa64899db972166a9b3568a6984c61bc
	CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++17
)

hunter_config(ethash VERSION 0.7.0-4576af36 URL https://${URL_BASE}/chfast/ethash/archive/4576af36f8ebb9bee2d5f04be692f295c64a7211.tar.gz
	SHA1 2001a265177c722b4cbe91c4160f3f582e0c9938
	CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++17
)

hunter_config(evmone VERSION 0.4.1-9eb137ea URL https://${URL_BASE}/FISCO-BCOS/evmone/archive/ea98055f1ebd216cdc89679ae64005e06aa620ae.tar.gz
	SHA1 8d3d499a2db398ca339ece20fd34ed463454b367
	CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++17 BUILD_SHARED_LIBS=OFF
)

hunter_config(
    Boost
    VERSION ${HUNTER_Boost_VERSION}
    CMAKE_ARGS
    CONFIG_MACRO=BOOST_UUID_RANDOM_PROVIDER_FORCE_POSIX
)
