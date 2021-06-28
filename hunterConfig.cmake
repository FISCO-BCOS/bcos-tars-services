# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/98a2530574ebf546fe38f92bddf3fe33305d9057.tar.gz"
    SHA1 be626ecd549151564dcec75cd4f2b2579188c237
    CMAKE_ARGS URL_BASE=${URL_BASE}
)
hunter_config(bcos-crypto VERSION 3.0.0-local-43df7523
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/25c8edb7d5cbadb514bbce9733573c8ffdb3600d.tar.gz"
    SHA1 4a1649e7095f5db58a5ae0671b2278bcccc25f1d
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/ee6dd57a668edaa0f5e67693c86d37183922860f.tar.gz"
    SHA1 1a05a49aa20bcf50261ef4dca9efa5098755e064
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-pbft VERSION 3.0.0-local-bddd4b4e
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/4cada40e01afec155ea06f574a7ccb2e8269be90.tar.gz"
    SHA1 e1e63f52de883fe3ee662408ad5cdb56e47b9a73
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/42b4c5a0b9a4472f62f4b20d7c7b02c831edee3c.tar.gz"
    SHA1 4b1082b008bcbc174053cbe88c1b709bf3f5375f
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-storage VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/b83313aac5fab7e0420c8b5777b3ca0a2342f436.tar.gz"
    SHA1 95ad73a304608a9f2022c34f0066f9b7975c606a
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/8cf0922ae5a0f0bb95ded2991f4e79ec92387e12.tar.gz"
    SHA1 c36f39055f43b93e351af024b0b7c429c0c4414d
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/15e18804aab90def4c2ba7d811024df921f935de.tar.gz"
    SHA1 1485c64a31b106f912aa1d1878da5d91dc0a2975
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/dee049993d276a238072cb24a3d98b038c4e8137.tar.gz"
    SHA1 d5e2661b6d0789f6bfa98ef0585c514c4abc4b42
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-dispatcher VERSION 3.0.0-local-2903b298
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-dispatcher/archive/4b9e0b3db816ae5902e79b4777637e6d2d114386.tar.gz"
    SHA1 4a1b6a063a079caf5b7062bb66187d387df3b3cc
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(bcos-executor VERSION 3.0.0-local-d02a7649
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-executor/archive/ac6d5d18bddfee86bcc41bedc5636bf7e11dc02e.tar.gz"
    SHA1 69bfbeeb058f07fc7af4139b4e6923c533a78305
    CMAKE_ARGS URL_BASE=${URL_BASE}
)

hunter_config(evmc VERSION 7.3.0-d951b1ef
		URL https://${URL_BASE}/FISCO-BCOS/evmc/archive/d951b1ef088be6922d80f41c3c83c0cbd69d2bfa.tar.gz
		SHA1 0b39b36cd8533c89ee0b15b6e94cff1111383ac7
        CMAKE_ARGS URL_BASE=${URL_BASE}
)
