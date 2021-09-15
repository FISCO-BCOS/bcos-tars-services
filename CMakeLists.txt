cmake_minimum_required(VERSION 3.15)
if (NOT DEFINED URL_BASE)
    set(URL_BASE "github.com")
   endif()
# set(URL_BASE "github.com.cnpmjs.org")
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
    URL "https://${URL_BASE}/FISCO-BCOS/hunter/archive/32b5a266ff0e808b2ed57afa3d351f7bb899c9f6.tar.gz"
    SHA1 "4fbc308a612c3ca3be7e03a54c4c4d27ba7804ed"
    FILEPATH "${BCOS_TARS_CMAKE_DIR}/config.cmake"
)

project(Bcos-Tars-Service)

# import dependencies
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
include(InstallBcosTarsProtocolDependencies)
include(ProjectBCOSExecutor)

# copy .clang-format into the project path
include(CopyClangFormat)

include_directories(${CMAKE_INSTALL_INCLUDEDIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

include(CompilerSettings)
# import tars project
add_subdirectory(libinitializer)

include(BuildInfoGenerator)

# import services
list(APPEND SERVICE_LIST StorageService FrontService GatewayService PBFTService ExecutorService DispatcherService TxPoolService RpcService)
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
    add_subdirectory(testClients)
endif()

# for code coverage
if (COVERAGE)
    include(Coverage)
    config_coverage("tars-cov" "'/usr*' '${CMAKE_CURRENT_SOURCE_DIR}/bcos-cmake-scripts*' '${CMAKE_CURRENT_SOURCE_DIR}/test/bcos-test*'")
endif ()
