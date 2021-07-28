# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/3e111b7ab7ce9db1911b4359710e0aa5923a4565.tar.gz"
    SHA1 c4e92cb2002b39edfccc2310d13ebd7e1800ae75
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON #DEBUG=ON
)

hunter_config(bcos-crypto
    VERSION 3.0.0-local-43df7523
    URL https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/25c8edb7d5cbadb514bbce9733573c8ffdb3600d.tar.gz
    SHA1 4a1649e7095f5db58a5ae0671b2278bcccc25f1d
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/7b3f222128e72515ed7c41d9b03da3742fecb435.tar.gz"
    SHA1 dfcf5c760797879e547e7463f6643fa661266201
)

hunter_config(bcos-pbft VERSION 3.0.0-local-a2a9f7d2
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/8438238c94edfbfa3628f53ca0d8f0f5314ef477.tar.gz"
    SHA1 5bd6802c1d539ee0afb76ad4163fd811c2f90470
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/ae1d1c46f6f965a017794e605a4c499cd6bb7c46.tar.gz"
    SHA1 f18897f732290377f62ec484e78a73bb687ad56a
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
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/6d8fbdbd4bf389e476e595fc782a8bfb56a90875.tar.gz"
    SHA1 a6409ecb9727c52078bd5934cc7fe4ac4d291583
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
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-dispatcher/archive/5aa203377bbfb3b64aaa632fdb16af813c6eb287.tar.gz"
    SHA1 8524bb4625e3c65d99a3ef14b33211e5d2fd7a6e
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