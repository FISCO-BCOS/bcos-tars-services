# Note: hunter_config takes effect globally, it is not recommended to set it in bcos-node, otherwise it will affect all projects that rely on bcos-framework
hunter_config(bcos-framework VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-framework/archive/e2193a04b6f5d6299839dc465b1e679802eb3675.tar.gz"
    SHA1 d1075dc7c3645dc9191b39d3931e8bd096a93fd4
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON #DEBUG=ON
)
hunter_config(bcos-crypto VERSION 3.0.0-local-43df7523
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-crypto/archive/25c8edb7d5cbadb514bbce9733573c8ffdb3600d.tar.gz"
    SHA1 4a1649e7095f5db58a5ae0671b2278bcccc25f1d
)

hunter_config(bcos-txpool VERSION 3.0.0-local-beda0a00
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-txpool/archive/1a3c7763e19263aad21a4c5ba57a0f028b5138e4.tar.gz"
    SHA1 3e93aeca3b6469cff4b51baaead3365fcb76a1fc
)

hunter_config(bcos-pbft VERSION 3.0.0-local-bddd4b4e
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-pbft/archive/01b455df5973da4458bd70ad3475f8a78ac19755.tar.gz"
    SHA1 dc97b56939d21a92ca257d1d10820f1c4efe075f
)

hunter_config(bcos-sync VERSION 3.0.0-local-50e0e264
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-sync/archive/432db818df69ae4fa57632aa4861642385eeda33.tar.gz"
    SHA1 188db879b47efa088bed5483c48108b1f11e37cb
)

hunter_config(bcos-storage VERSION 3.0.0-local
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-storage/archive/b83313aac5fab7e0420c8b5777b3ca0a2342f436.tar.gz"
    SHA1 95ad73a304608a9f2022c34f0066f9b7975c606a
)

hunter_config(bcos-ledger
    VERSION 3.0.0-local-1956c515f
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-ledger/archive/8cf0922ae5a0f0bb95ded2991f4e79ec92387e12.tar.gz"
    SHA1 c36f39055f43b93e351af024b0b7c429c0c4414d
)

hunter_config(bcos-front VERSION 3.0.0-local-2ed687bb
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-front/archive/15e18804aab90def4c2ba7d811024df921f935de.tar.gz"
    SHA1 1485c64a31b106f912aa1d1878da5d91dc0a2975
)

hunter_config(bcos-gateway VERSION 3.0.0-local-1fb592e4
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-gateway/archive/dee049993d276a238072cb24a3d98b038c4e8137.tar.gz"
    SHA1 d5e2661b6d0789f6bfa98ef0585c514c4abc4b42
)

hunter_config(bcos-dispatcher VERSION 3.0.0-local-2903b298
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-dispatcher/archive/4fbc714b9ce38da0b8bc3a1e051dde53563113e3.tar.gz"
    SHA1 6d027b862f5b39f94e6fc68aa8234d6723f3b6f6
)

hunter_config(bcos-executor VERSION 3.0.0-local-ac6d5d18
    URL "https://${URL_BASE}/FISCO-BCOS/bcos-executor/archive/ac6d5d18bddfee86bcc41bedc5636bf7e11dc02e.tar.gz"
    SHA1 69bfbeeb058f07fc7af4139b4e6923c533a78305
    CMAKE_ARGS HUNTER_PACKAGE_LOG_BUILD=ON HUNTER_PACKAGE_LOG_INSTALL=ON #DEBUG=ON
)

hunter_config(evmc VERSION 7.3.0-d951b1ef
		URL https://${URL_BASE}/FISCO-BCOS/evmc/archive/d951b1ef088be6922d80f41c3c83c0cbd69d2bfa.tar.gz
		SHA1 0b39b36cd8533c89ee0b15b6e94cff1111383ac7
)
