cmake_minimum_required(VERSION 3.15)

# import dependencies
include (Dependencies.cmake)

include(HunterGate)
HunterGate(
    URL "https://${GIT_URL_BASE}/FISCO-BCOS/hunter/archive/8c81ea454c0c268b257da1fe3e3f2f69bd59a67d.tar.gz"
    SHA1 "13292adf48c4c053ff2ceb7173631a296c08d1ac"
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

if(TESTS)
    enable_testing()
    add_subdirectory(tests)
    add_subdirectory(testClients)
endif()