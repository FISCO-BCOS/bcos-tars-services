# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
    URL https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/d8f6a1399e27ab2ec15fd65644ed812bd1ca5c9c.tar.gz
    SHA1 56f381cce3dc5a88741a7f452b5f86addbe6b310
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON #DEBUG=ON
)

hunter_config(wedpr-crypto VERSION 1.1.0-10f314de
	URL https://${URL_BASE}/WeBankBlockchain/WeDPR-Lab-Crypto/archive/10f314de45ec31ce9e330922b522ce173662ed33.tar.gz
	SHA1 626df59f87ea2c6bb5128f7d104588179809910b
	CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=OFF HUNTER_PACKAGE_LOG_INSTALL=ON
)

hunter_config(bcos-crypto
    VERSION 3.0.0-local-43df7523
    URL https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/255002b047b359a45c953d1dab29efd2ff6eb080.tar.gz
    SHA1 4d02de20be1f9bf79d762c5b8686368286504e07
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/f51acb2a608e75fc255e8fcb3a4544a46e586832.tar.gz"
    SHA1 28bbc2cc25862889bf32580c26152cc9ac0601e6
)

hunter_config(bcos-pbft VERSION 3.0.0-local-a2a9f7d2
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/522bf8bf91383cee494a7037bf3cba0e2bd642b7.tar.gz"
    SHA1 5f7c6dd3e69c511c8e4140842971a7c679b6a51d
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/f5344ba8425ff251c74eb4122035d38a2a612e90.tar.gz"
    SHA1 b284d0755db6f66e164ff2e8d7cb48cced375a9c
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
    CMAKE_ARGS URL_BASE=${URL_BASE} CMAKE_BUILD_TYPE=Debug
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/e18c81e1613c142c1efb8e55439e20cc953ed345.tar.gz"
    SHA1 432cdd9b3f8f4517006cee19175258adc259a33d
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/f0344dee37773b2689b1178f2681fd665dd9d4be.tar.gz"
    SHA1 30e19619e0042c458b9e5ea786185cad273276f9
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/1fbe1b7d4eb0e573a753a929840452dc0e638380.tar.gz"
    SHA1 87629e62572a8193afd152f41ca290f8ddd80314
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-dispatcher VERSION 3.0.0-local-2903b298
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-dispatcher/archive/257bd6787198599d5410596c7ed0c98964c44653.tar.gz"
    SHA1 b93ff9d34b732e5910e4a88655d0b0806c52b3c9
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-rpc VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-rpc/archive/12909c8f1184d9bcbc1f67727cb3713a4a671494.tar.gz"
    SHA1 e20d6e4fbee4a9ad22abe041c9a4cf0c793faecf
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

# hunter_config(bcos-executor VERSION 3.0.0-local-ac6d5d18
#     URL "https://${URL_BASE}/FISCO-BCOS/bcos-executor/archive/8a2dc4e32fc4de6a9a95ed2fbc83540e7ce589b4.tar.gz"
#     SHA1 c8254cd8a943ea21ab2bdbacd5e54d50ad20f778
#     CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON DEBUG=ON
# )

hunter_config(evmc VERSION 7.3.0-c7feb13f
	URL https://${URL_BASE}/FISCO-BCOS/evmc/archive/c7feb13f582919242da9f4f898ed4578785c9ecc.tar.gz
	SHA1 28ab1c74dd3340efe101418fd5faf19d34c9f7a9
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(intx VERSION 0.4.1 URL https://${URL_BASE}/chfast/intx/archive/v0.4.0.tar.gz
    SHA1 8a2a0b0efa64899db972166a9b3568a6984c61bc
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
