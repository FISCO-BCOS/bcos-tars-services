# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
    URL "https://${URL_BASE}/cyjseagull/bcos-framework/archive/c1079bbd5bb57c473e468dcf73f5d4e11ca4da20.tar.gz"
    SHA1 fbc3c3352317ab27255e63df579482a3ec5a68dd
)

hunter_config(bcos-crypto
    VERSION 3.0.0-local-43df7523
    URL https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/25c8edb7d5cbadb514bbce9733573c8ffdb3600d.tar.gz
    SHA1 4a1649e7095f5db58a5ae0671b2278bcccc25f1d
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL "https://${URL_BASE}/cyjseagull/bcos-txpool/archive/079af98cd49362e83e095107e80138479781b3d2.tar.gz"
    SHA1 3e8a9a641ea41ec6bc4a7d753fe719b1c3d317df
)

hunter_config(bcos-pbft VERSION 3.0.0-local-a2a9f7d2
    URL "https://${URL_BASE}/cyjseagull/bcos-pbft/archive/a1b9a65bdc18376b658b7c4e7734ff0c870f7d22.tar.gz"
    SHA1 39a128f379b299165884195c669375b4785311f8
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL "https://${URL_BASE}/cyjseagull/bcos-sync/archive/c515c44f52a7f7e020a4ac9b9e0d6da50467d494.tar.gz"
    SHA1 42f98b92ed202ae6a07e88fad0ac416e56783921
)

hunter_config(rocksdb VERSION 6.20.3
	URL https://${URL_BASE}/facebook/rocksdb/archive/refs/tags/v6.20.3.tar.gz
    SHA1 64e4e6031820026c051d6e2072c0197e3bce1643
    CMAKE_ARGS WITH_TESTS=OFF WITH_GFLAGS=OFF WITH_BENCHMARK_TOOLS=OFF WITH_CORE_TOOLS=OFF
    WITH_TOOLS=OFF PORTABLE=ON FAIL_ON_WARNINGS=OFF WITH_ZSTD=ON
)

hunter_config(bcos-storage VERSION 3.0.0-local-c4dfb130
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/7584b308b4ffd3d712d8a1bb0578052c379e4bc6.tar.gz"
    SHA1 6b75e684d135a7a11317eef72126be43c6a7ec72
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/fe0e191309e93ae5152705a8f93c632887085962.tar.gz"
    SHA1 985b7ff31bd0e66dfe17745c37017a7888333965
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/15e18804aab90def4c2ba7d811024df921f935de.tar.gz"
    SHA1 1485c64a31b106f912aa1d1878da5d91dc0a2975
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/380ee609e9cbc88f0f5f5dcd13fd6c3c69c675ef.tar.gz"
    SHA1 5c1c97b7ae6a08e7bba7d0f0c4757765e7a2e42c
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-dispatcher VERSION 3.0.0-local-2903b298
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-dispatcher/archive/a7675d21797f201c8f159a3c9416a83e8098c8ff.tar.gz"
    SHA1 a909e1cf1875940583287c31d659c4390b7f1ea6
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