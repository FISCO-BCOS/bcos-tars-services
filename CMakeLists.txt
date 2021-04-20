cmake_minimum_required(VERSION 3.15)

set(GIT_URL_BASE "github.com.cnpmjs.org")
# import bcos-cmake-scripts & hunter & bcos-framework
include(FetchContent)
FetchContent_Declare(bcos-cmake-scripts
GIT_REPOSITORY https://${GIT_URL_BASE}/FISCO-BCOS/bcos-cmake-scripts.git
GIT_TAG dev
SOURCE_DIR bcos-cmake-scripts
)
FetchContent_MakeAvailable(bcos-cmake-scripts)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR}/bcos-cmake-scripts)

FetchContent_Declare(bcos-framework
GIT_REPOSITORY https://${GIT_URL_BASE}/FISCO-BCOS/bcos-framework.git
GIT_TAG dev
SOURCE_DIR bcos-framework
# BUILD_IN_SOURCE 1
)
FetchContent_MakeAvailable(bcos-framework)

project(Tars-Service)

if(WIN32)
    include (c:\\tars\\cpp\\makefile\\tars-tools.cmake)
else()
    include (/usr/local/tars/cpp/makefile/tars-tools.cmake)
endif()

####you can: cd build; cmake .. -DTARS_WEB_HOST={your web host}
set(TARS_WEB_HOST "" CACHE STRING "set web host")
IF (TARS_WEB_HOST STREQUAL "")
	set(TARS_WEB_HOST "http://tars.test.com")
ENDIF ()

include_directories(/usr/local/tars/cpp/thirdparty/include)
link_directories(/usr/local/tars/cpp/thirdparty/lib)

include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR}/tars)

#include_directories(/home/tarsprotol/App/OtherServer)

# import tars project
add_subdirectory(tars)
add_subdirectory(protocols)
add_subdirectory(LedgerService)
add_subdirectory(StorageService)
