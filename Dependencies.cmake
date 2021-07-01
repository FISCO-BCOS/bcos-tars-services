cmake_minimum_required(VERSION 3.15)
include(ExternalProject)
ExternalProject_Add(tarscpp
    GIT_REPOSITORY https://${URL_BASE}/TarsCloud/TarsCpp.git
    GIT_TAG release/2.4
    SOURCE_DIR ${DEPENDENCIES_DIR}/tarscpp
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_DIR}/tarscpp-install
    PATCH_COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/patchTars.sh ${DEPENDENCIES_DIR}/tarscpp ${DEPENDENCIES_DIR}/tarscpp-install
)
include_directories(${DEPENDENCIES_DIR}/tarscpp-install/include)
link_directories(${DEPENDENCIES_DIR}/tarscpp-install/lib)
set(TARS2CPP ${DEPENDENCIES_DIR}/tarscpp-install/tools/tars2cpp)

include(InstallBcosFrameworkDependencies)
include(InstallBcosCryptoDependencies)
include(InstallBcosTxPoolDependencies)
include(InstallBcosPBFTDependencies)
include(InstallBcosStorageDependencies)
include(InstallBcosLedgerDependencies)
include(InstallBcosSyncDependencies)
include(InstallBcosFrontDependencies)
include(InstallBcosGateWayDependencies)
include(InstallBcosDispatcherDependencies)
hunter_add_package(evmc)
set(EXECUTOR_URL https://${URL_BASE}/FISCO-BCOS/bcos-executor/archive/06c2bd7bae00c3a3eb9e0028fe2cb0c9c378e43f.tar.gz)
set(EXECUTOR_URL_SHA256 53d8d36d9a9ad9623c4ee942cc071bc4a7e8aea1213b245c9cbdb94d64579f1b)
include(ProjectExecutor)