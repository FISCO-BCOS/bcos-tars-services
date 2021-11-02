# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
	URL https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/79a6e580c61a1add5fabea51b10e1524c2cdc972.tar.gz
    SHA1 ee17224b750a4c2ae53884e847c13d2eff028364
	CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON
)

hunter_config(bcos-crypto
    VERSION 3.0.0-local-43df7523
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/255002b047b359a45c953d1dab29efd2ff6eb080.tar.gz
    SHA1 4d02de20be1f9bf79d762c5b8686368286504e07
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-boostssl
	VERSION 3.0.0-local
	URL "https://${URL_BASE}/FISCO-BCOS/bcos-boostssl/archive/176dbb74c41a0efaf38bd8725908929b708bdf14.tar.gz"
	SHA1 ab851a11610fab6df520730a3bcbf8bd08a87065
)

hunter_config(bcos-tars-protocol
    VERSION 3.0.0-local
    URL https://${URL_BASE}/FISCO-BCOS/bcos-tars-protocol/archive/325b1c6971e3f42f527996cdf990b722a5eb4d50.tar.gz
    SHA1 06b904725472b1063c6cfc34894cd1979a22ad8a
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON URL_BASE=${URL_BASE}
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/2cfbea4f3eeb2fe28425ae77714f387d4b8e111c.tar.gz
    SHA1 6a53ef19ecfd6cc9bee2470da0a99854dc1760bd
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-pbft VERSION 3.0.0-local-a2a9f7d2
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/edaccb487486927237224bc5ec1c6d093184222f.tar.gz
    SHA1 4892dd3eb434f4f29b0023d3ffa07435fa577613
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/eb80875a50b6dd6fdb834d6a682afa63843f8e13.tar.gz
    SHA1 dcdc85bdda550b168751bc91babd9f66edae6188
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(rocksdb VERSION 6.20.3
	URL  https://${URL_BASE}/facebook/rocksdb/archive/refs/tags/v6.20.3.tar.gz
    SHA1 64e4e6031820026c051d6e2072c0197e3bce1643
    CMAKE_ARGS WITH_TESTS=OFF WITH_GFLAGS=OFF WITH_BENCHMARK_TOOLS=OFF WITH_CORE_TOOLS=OFF
    WITH_TOOLS=OFF PORTABLE=ON FAIL_ON_WARNINGS=OFF WITH_ZSTD=ON BUILD_SHARED_LIBS=OFF
)

hunter_config(bcos-storage VERSION 3.0.0-local-900836da
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/6c34b28b581a92c2f77c8e0b527995118751a0ef.tar.gz
    SHA1 5e91a08e8d17bd40f06f8c6db8d7d4c45b2509af
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/e501909b59a674f78965245d8778bb15ffba1c8d.tar.gz
    SHA1 44fa4ad0594907a4b4d6942fbff084b1dfa1d666
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/10880d313161ae12e6e5d5bc63876fd274c4bb7c.tar.gz
    SHA1 e9c0cd550f411a5f577f9e2e97ad395d502b2fb3
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL  https://${URL_BASE}/cyjseagull/bcos-gateway/archive/00cc1160c382ef83f51effeac4d27e8c31bb4771.tar.gz
    SHA1 575607202037f4a4ce18ee082020907a7fa623d9
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-scheduler VERSION 3.0.0-local-b1c450b95b6f83e6a0cf3bd2f385c2ddef4921b4
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-scheduler/archive/5b783170975ae609ce62b42e3c8daa8cb0689d34.tar.gz
    SHA1 7e73ea40d075b0fff5ab1c800fff25dd07af0486
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-rpc VERSION 3.0.0-local
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-rpc/archive/733e2ef706c8909dd35742f0b14e2ba8e559c772.tar.gz
    SHA1 e3ccd89a90e6c283ff98d51186c1168e9c5a4975
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
