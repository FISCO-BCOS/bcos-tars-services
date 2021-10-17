cmake_minimum_required(VERSION 3.15)
if (NOT DEFINED URL_BASE)
    set(URL_BASE "github.com")
   endif()
set (DEPENDENCIES_DIR ${CMAKE_CURRENT_SOURCE_DIR}/deps/src)

set(BCOS_TARS_CMAKE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/cmake" CACHE PATH "The cmake path for the library")
set(BCOS_CMAKE_SCRIPTS_DIR ${CMAKE_CURRENT_BINARY_DIR}/bcos-cmake-scripts)
include(FetchContent)
FetchContent_Declare(bcos-cmake-scripts
    GIT_REPOSITORY https://${URL_BASE}/FISCO-BCOS/bcos-cmake-scripts.git
    GIT_TAG dev
    SOURCE_DIR ${BCOS_CMAKE_SCRIPTS_DIR}
)
FetchContent_MakeAvailable(bcos-cmake-scripts)
list(APPEND CMAKE_MODULE_PATH ${BCOS_CMAKE_SCRIPTS_DIR})
list(APPEND CMAKE_MODULE_PATH ${BCOS_TARS_CMAKE_DIR})

include(HunterGate)
HunterGate(
    URL "https://${URL_BASE}/FISCO-BCOS/hunter/archive/b55a347258b0fc3eda47f4ccd3e2c0db595b4957.tar.gz"
    SHA1 "9331d1e70cee5498bd59ada13aa68278cbb9a2d6"
    FILEPATH "${BCOS_TARS_CMAKE_DIR}/config.cmake"
)

project(Bcos-Tars-Service)

# import dependencies
include(InstallBcosFrameworkDependencies)
include(InstallBcosCryptoDependencies)
include(InstallBcosTarsProtocolDependencies)
include(InstallBcosGateWayDependencies)
include(InstallBcosRpcDependencies)

include(InstallBcosTxPoolDependencies)
include(InstallBcosPBFTDependencies)
include(InstallBcosStorageDependencies)
include(InstallBcosLedgerDependencies)
include(InstallBcosSyncDependencies)
include(InstallBcosFrontDependencies)
include(InstallBcosScheduler)
include(InstallBcosGroupManager)
include(ProjectBCOSExecutor)

# copy .clang-format into the project path
include(CopyClangFormat)

include_directories(${CMAKE_INSTALL_INCLUDEDIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include(CompilerSettings)

add_subdirectory(libinitializer)

set(PBFT_INITIALIZER_LIB PBFTInitializer)
set(TXPOOL_INITIALIZER_LIB txPoolInitializer)
set(FRONT_INITIALIZER_LIB frontServiceInitializer)

include(BuildInfoGenerator)
# import services
list(APPEND SERVICE_LIST GroupManagerService FrontService GatewayService RpcService TxPoolService PBFTService NativeNode)
# TODO:
# DispatcherService, ExecutorService, 

foreach(SERVICE ${SERVICE_LIST})
    add_subdirectory(${SERVICE})

    add_custom_command(OUTPUT ${SERVICE}.tgz
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR} tar czfv ${SERVICE}.tgz ${SERVICE}/${SERVICE}
    COMMENT "Compressing ${SERVICE}...")
    add_custom_target(${SERVICE}-tar DEPENDS ${SERVICE}.tgz ${SERVICE})

    list(APPEND SERVICE_TAR_LIST ${SERVICE}-tar)
endforeach()
add_custom_target(tar DEPENDS ${SERVICE_TAR_LIST})

if(TEST)
    enable_testing()
    add_subdirectory(tests)
endif()

# for code coverage
if (COVERAGE)
    include(Coverage)
    config_coverage("tars-cov" "'/usr*' '${CMAKE_CURRENT_SOURCE_DIR}/bcos-cmake-scripts*' '${CMAKE_CURRENT_SOURCE_DIR}/test/bcos-test*'")
endif ()
