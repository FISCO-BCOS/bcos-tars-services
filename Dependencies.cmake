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
set(EXECUTOR_URL https://${URL_BASE}/FISCO-BCOS/bcos-executor/archive/dbb155996425fbfff8219fae9a1ca34ae14149fc.tar.gz)
set(EXECUTOR_URL_SHA256 be426f734e69319a1d0289a91675187b3712d16a6368f2c605684a177adb5877)
include(ProjectExecutor)