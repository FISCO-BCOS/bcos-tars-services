cmake_minimum_required(VERSION 3.15)
include(ExternalProject)
ExternalProject_Add(tarscpp
    GIT_REPOSITORY https://${URL_BASE}/TarsCloud/TarsCpp.git
    GIT_TAG 1ad926f945d55963eb11c0e7374eb04f4147e014
    URL_HASH sha256=113aafcd968fec71aebb1203e13a003b06cd93f0745ca43527262788afcf254e
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
set(EXECUTOR_URL https://${URL_BASE}/FISCO-BCOS/bcos-executor/archive/fea320afb54a6e3344a5c20b573556ed492e9a3f.tar.gz)
set(EXECUTOR_URL_SHA256 a117e27fd2f0d74620678f73d33c1ff8f3bb13cf542031c605d2eb21586fe6fc)
include(ProjectExecutor)