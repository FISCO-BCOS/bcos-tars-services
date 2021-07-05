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
    URL "https://${URL_BASE}/FISCO-BCOS/hunter/archive/f88ce89a9fcd557b7fb2aa0f7abaa3c541dc6903.tar.gz"
    SHA1 "b7792231bf64539602e696e32bfc163c9bc1812a"
    FILEPATH "${CMAKE_SOURCE_DIR}/hunterConfig.cmake"
)

project(Bcos-Tars-Service)

# import dependencies
include (Dependencies.cmake)

# copy .clang-format into the project path
include(CopyClangFormat)

set(TARS_GENERATE_DIR ${CMAKE_CURRENT_BINARY_DIR})
include_directories(${TARS_GENERATE_DIR}/tarscpp-install/thirdparty/include)
link_directories(${TARS_GENERATE_DIR}/tarscpp-install/thirdparty/lib)

set(TARS_HEADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/tars-generated)
include_directories(${TARS_HEADER_DIR})

include(CompilerSettings)
# import tars project
add_subdirectory(tars)
add_subdirectory(protocols)
add_subdirectory(libinitializer)

# add_subdirectory(StorageService)
# add_subdirectory(FrontService)
# add_subdirectory(GatewayService)
# add_subdirectory(PBFTService)
# add_subdirectory(ExecutorService)
# add_subdirectory(DispatcherService)
# add_subdirectory(TxPoolService)

# import services
list(APPEND SERVICE_LIST StorageService FrontService GatewayService PBFTService ExecutorService DispatcherService TxPoolService)
foreach(SERVICE ${SERVICE_LIST})
    add_subdirectory(${SERVICE})

    add_custom_command(OUTPUT ${SERVICE}.tgz
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMAND cmake -E chdir ${CMAKE_BINARY_DIR} tar czfv ${SERVICE}.tgz -C ${SERVICE}/ ${SERVICE}Server
    COMMENT "Compressing ${SERVICE}...")
    add_custom_target(${SERVICE}-tar DEPENDS ${SERVICE}.tgz ${SERVICE}Server)

    list(APPEND SERVICE_TAR_LIST ${SERVICE}-tar)
endforeach()

add_custom_target(tar DEPENDS ${SERVICE_TAR_LIST})

if(TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(testClients)
endif()

# for code coverage
if (COVERAGE)
    include(Coverage)
    config_coverage("coverage" "'/usr*' '${CMAKE_CURRENT_SOURCE_DIR}/bcos-cmake-scripts*' '${CMAKE_CURRENT_SOURCE_DIR}/test/bcos-test*'")
endif ()
