cmake_minimum_required(VERSION 3.15)

# import dependencies
include (Dependencies.cmake)

include(HunterGate)
HunterGate(
    URL "https://${GIT_URL_BASE}/FISCO-BCOS/hunter/archive/8c81ea454c0c268b257da1fe3e3f2f69bd59a67d.tar.gz"
    SHA1 "13292adf48c4c053ff2ceb7173631a296c08d1ac"
)

project(Bcos-Tars-Service)

if(WIN32)
    include (c:\\tars\\cpp\\makefile\\tars-tools.cmake)
else()
    include (/usr/local/tars/cpp/makefile/tars-tools.cmake)
endif()

#####you can: cd build; cmake .. -DTARS_WEB_HOST={your web host}
set(TARS_WEB_HOST "" CACHE STRING "set web host")
IF (TARS_WEB_HOST STREQUAL "")
	set(TARS_WEB_HOST "http://tars.test.com")
ENDIF ()

include_directories(/usr/local/tars/cpp/thirdparty/include)
link_directories(/usr/local/tars/cpp/thirdparty/lib)

include_directories(${CMAKE_SOURCE_DIR}/tars)

#include_directories(/home/tarsprotol/App/OtherServer)
include(CompilerSettings)

hunter_add_package(Microsoft.GSL)
hunter_add_package(Boost COMPONENTS all)

# import tars project
add_subdirectory(tars)
add_subdirectory(protocols)
add_subdirectory(StorageService)

if(TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()