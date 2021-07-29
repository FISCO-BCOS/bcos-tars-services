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
include(InstallBcosRpcDependencies)
include(InstallBcosDispatcherDependencies)

include(ProjectBCOSExecutor)