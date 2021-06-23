cmake_minimum_required(VERSION 3.15)

# import dependencies
include (Dependencies.cmake)

include(HunterGate)
HunterGate(
    URL "https://$${GIT_URL_BASE}/FISCO-BCOS/hunter/archive/7268240de829ba351c98d574db9dd7dfddb1ad2e.tar.gz"
    SHA1 "29a725d3fac8b23f255530b7e40ffb47b34973f9"
    FILEPATH "${CMAKE_SOURCE_DIR}/hunterConfig.cmake"
)

project(Bcos-Tars-Service)

include_directories(${DEPENDENCIES_DIR}/tarscpp-install/thirdparty/include)
link_directories(${DEPENDENCIES_DIR}/tarscpp-install/thirdparty/lib)

set(TARS_HEADER_DIR ${CMAKE_CURRENT_BINARY_DIR}/tars-generated)
include_directories(${TARS_HEADER_DIR})

include(CompilerSettings)

# import tars project
add_subdirectory(tars)
add_subdirectory(protocols)
add_subdirectory(StorageService)
add_subdirectory(FrontService)
add_subdirectory(GatewayService)
add_subdirectory(PBFTService)
add_subdirectory(ExecutorService)

if(TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(testClients)
endif()