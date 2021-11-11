# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
	URL https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/e4b2da37e26d269c7ab64f2896ef81d85c1a04bf.tar.gz
    SHA1 4f895c96da870e127962dd34ebef6c90afc6d29e
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
	URL "https://${URL_BASE}/FISCO-BCOS/bcos-boostssl/archive/08f2ad5a5dea2827478c60d9aaddc08347e3ad79.tar.gz"
	SHA1 afd2ae3a0c66aa2e238b1be7ce42e24ddc3fa43d
)

hunter_config(bcos-tars-protocol
    VERSION 3.0.0-local
    URL https://${URL_BASE}/FISCO-BCOS/bcos-tars-protocol/archive/a44c56e4312aed7d5d7ca074493bac37c9f2f811.tar.gz
    SHA1 f3d55b11360b1e0db04539d5e20ee92608f8b11a
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON URL_BASE=${URL_BASE}
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/2cfbea4f3eeb2fe28425ae77714f387d4b8e111c.tar.gz
    SHA1 6a53ef19ecfd6cc9bee2470da0a99854dc1760bd
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-pbft VERSION 3.0.0-local-a2a9f7d2
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/2b37b7b6818cfcd5a4b5bd4fca243e3443bf9195.tar.gz
    SHA1 8d86774daf48f99e1f3649454659838c3f19be48
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/4ed9d0f2dfd3c2e7013e5dc26aa8f43f7c0e4674.tar.gz
    SHA1 1f0fe4f6de34fd6a371f65fdc01d07c8fcf504cc
    CMAKE_ARGS URL_BASE=${URL_BASE}  HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(rocksdb VERSION 6.20.3
	URL  https://${URL_BASE}/facebook/rocksdb/archive/refs/tags/v6.20.3.tar.gz
    SHA1 64e4e6031820026c051d6e2072c0197e3bce1643
    CMAKE_ARGS WITH_TESTS=OFF WITH_GFLAGS=OFF WITH_BENCHMARK_TOOLS=OFF WITH_CORE_TOOLS=OFF
    WITH_TOOLS=OFF PORTABLE=ON FAIL_ON_WARNINGS=OFF WITH_ZSTD=ON BUILD_SHARED_LIBS=OFF
)

hunter_config(bcos-storage VERSION 3.0.0-local-900836da
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/4779f7d7c1f89b2a9974495dab950601e547ebdf.tar.gz
    SHA1 2a84dd92c940a400d6686fb06d4b2cb49f681653
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/f2ba3ace3d31c9b2af3f59d09e6d004c012b5780.tar.gz
    SHA1 b9fce286e87be01f310cf3a5ab311fe7fdabc540
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/bccb8f63b29d9d3d70a2ccfd8811764195b512b2.tar.gz
    SHA1 61138c66dd95c06eb994e9a1fdad47cfa8739d62
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/6dbeed20ecd00b90c5086007cb9a81b949807d4b.tar.gz
    SHA1 abc3a2e16bb1d71a25cc82efc1fef3340b8aa118
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-scheduler VERSION 3.0.0-local-b1c450b95b6f83e6a0cf3bd2f385c2ddef4921b4
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-scheduler/archive/90d14bb010d1a63f68ecd88014a973f0803396ea.tar.gz
    SHA1 476164af6c3209bf3903c43f19d9283a2b3f5648
    CMAKE_ARGS URL_BASE=${URL_BASE} HUNTER_KEEP_PACKAGE_SOURCES=ON
)

hunter_config(bcos-rpc VERSION 3.0.0-local
    URL  https://${URL_BASE}/FISCO-BCOS/bcos-rpc/archive/bbf3c9f45a484e5afcdc1618127af90f6b6df86d.tar.gz
    SHA1 606a0fffe42391a5905bc407cf007843f83dc93b
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
