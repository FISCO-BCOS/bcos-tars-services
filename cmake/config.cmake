# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.1-local
	URL  https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/0a747e0c7d7ddf7b7b68b09a1877ecc06a5ce5df.tar.gz
	SHA1 caa9c617a415a4c437b546f44a6544a4d32f4357
	CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-crypto
    VERSION 3.0.0-local-43df7524
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/255002b047b359a45c953d1dab29efd2ff6eb080.tar.gz
    SHA1 4d02de20be1f9bf79d762c5b8686368286504e07
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-boostssl
	VERSION 3.0.0-local
	URL "https://${URL_BASE}/FISCO-BCOS/bcos-boostssl/archive/1b972a6734ef49ac4ca56184d31fe54a08a97e82.tar.gz"
	SHA1 6d35d940eacb7f41db779fb8182cbebf50535574
)

hunter_config(bcos-tars-protocol
    VERSION 3.0.0-local
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-tars-protocol/archive/4cc312d213256bc173eea72873c5237b9714c009.tar.gz
    SHA1 a6a18e6133d97e141341a55e947ad60f04be1bc0
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/892165ce45dcfa12d5964d52461760bbb7f1f212.tar.gz
    SHA1 dbb714f32e4ed61b782763dd952f3dac4f368730
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-pbft VERSION 3.0.0-local-a2a9f7d2
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/4f872ae44640ec3ab6215547c707eb3c0ed43544.tar.gz
    SHA1 f37ee10223de8728a17104d63776981777be8341
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/c1461c001e39fe7868c5f6ad8ef8686fe05a7684.tar.gz
    SHA1 9ec552f515709981be8b12e7945c9a21435226ca
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(rocksdb VERSION 6.20.3
	URL  https://${URL_BASE}/facebook/rocksdb/archive/refs/tags/v6.20.3.tar.gz
    SHA1 64e4e6031820026c051d6e2072c0197e3bce1643
    CMAKE_ARGS WITH_TESTS=OFF WITH_GFLAGS=OFF WITH_BENCHMARK_TOOLS=OFF WITH_CORE_TOOLS=OFF
    WITH_TOOLS=OFF PORTABLE=ON FAIL_ON_WARNINGS=OFF WITH_ZSTD=ON BUILD_SHARED_LIBS=OFF
)

hunter_config(bcos-storage VERSION 3.0.0-local-900836da
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/485ea43bf4b205faca8da96fb12e2f8b93fcf297.tar.gz
    SHA1 90d5241e524c9cc2e64e35f767955d8268868919
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/8fd0ed5cabc9c660790339e95fcfebdb53a2bcaa.tar.gz
    SHA1 66844b538ce7eb45140b9a9922561300bedf5fbf
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/10880d313161ae12e6e5d5bc63876fd274c4bb7c.tar.gz
    SHA1 e9c0cd550f411a5f577f9e2e97ad395d502b2fb3
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/ec2642e55daf114d97272e354f24869a5575ab1b.tar.gz
    SHA1 9cd6608f882c6d745a8f3af6390ed629d4df43ea
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-scheduler VERSION 3.0.0-local-b1c450b95b6f83e6a0cf3bd2f385c2ddef4921b4
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-scheduler/archive/e3df258f2b7b29fd7245ab6599131f3a17b17513.tar.gz
    SHA1 1b351a9e61df408c6b94ed7403d3297eb7e83493
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-rpc VERSION 3.0.0-local
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-rpc/archive/c2bd8e2f594867fa082c3aef0fb46fd53d7d9221.tar.gz
    SHA1 0a1a940c832532610ee65550c8fe3aaebc22f010
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(evmc VERSION 7.3.0-c7feb13f
	URL  https://${URL_BASE}/FISCO-BCOS/evmc/archive/c7feb13f582919242da9f4f898ed4578785c9ecc.tar.gz
	SHA1 28ab1c74dd3340efe101418fd5faf19d34c9f7a9
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(intx VERSION 0.4.1 URL https://${URL_BASE}/chfast/intx/archive/v0.4.0.tar.gz
    SHA1  8a2a0b0efa64899db972166a9b3568a6984c61bc
	CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++17
)

hunter_config(ethash VERSION 0.7.0-4576af36 URL https://${URL_BASE}/chfast/ethash/archive/4576af36f8ebb9bee2d5f04be692f295c64a7211.tar.gz
	SHA1 2001a265177c722b4cbe91c4160f3f582e0c9938
	CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++17
)

hunter_config(evmone VERSION 0.4.1-b726a1e1
	URL https://${URL_BASE}/FISCO-BCOS/evmone/archive/b726a1e1722109291ac18bc7c5fad5aac9d2e8c5.tar.gz
	SHA1 e41fe0514a7a49a9a5e7eeb1b42cf2c3ced67f5d
	CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++17 BUILD_SHARED_LIBS=OFF
)

hunter_config(
    Boost
    VERSION ${HUNTER_Boost_VERSION}
    CMAKE_ARGS
    CONFIG_MACRO=BOOST_UUID_RANDOM_PROVIDER_FORCE_POSIX
)

hunter_config(ZLIB VERSION ${HUNTER_ZLIB_VERSION} CMAKE_ARGS CMAKE_POSITION_INDEPENDENT_CODE=TRUE)
hunter_config(c-ares VERSION 1.14.0-p0 CMAKE_ARGS CMAKE_POSITION_INDEPENDENT_CODE=TRUE CARES_BUILD_TOOLS=OFF)
hunter_config(re2 VERSION ${HUNTER_re2_VERSION} CMAKE_ARGS CMAKE_POSITION_INDEPENDENT_CODE=TRUE RE2_BUILD_TESTING=OFF)
hunter_config(abseil VERSION ${HUNTER_abseil_VERSION} CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++11 CMAKE_POSITION_INDEPENDENT_CODE=TRUE ABSL_ENABLE_INSTALL=ON ABSL_RUN_TESTS=OFF)
hunter_config(gRPC VERSION ${HUNTER_gRPC_VERSION} CMAKE_ARGS CMAKE_CXX_FLAGS=-std=c++17 CMAKE_POSITION_INDEPENDENT_CODE=TRUE)