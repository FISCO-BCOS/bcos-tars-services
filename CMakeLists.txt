cmake_minimum_required(VERSION 3.15)

# import dependencies
include (Dependencies.cmake)

include(HunterGate)
HunterGate(
    URL "https://github.com/FISCO-BCOS/hunter/archive/d73ee00ba165b521f0e63161119fbf18a7eb6e60.tar.gz"
    SHA1 "baeb7665f6755232e86bb1c86974b2000a1c4c8e"
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
add_subdirectory(LedgerService)
add_subdirectory(StorageService)

if(TESTS)
    add_subdirectory(tests)
    add_subdirectory(testClient)
endif()