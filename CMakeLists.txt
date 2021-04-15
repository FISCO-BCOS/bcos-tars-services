cmake_minimum_required(VERSION 3.15)

# import bcos-cmake-scripts & hunter & bcos-framework
include(FetchContent)
FetchContent_Declare(bcos-cmake-scripts
GIT_REPOSITORY https://gitee.com/FISCO-BCOS/bcos-cmake-scripts.git
GIT_TAG dev
SOURCE_DIR bcos-cmake-scripts
)
FetchContent_MakeAvailable(bcos-cmake-scripts)

include(${CMAKE_BINARY_DIR}/bcos-cmake-scripts/HunterGate.cmake)
HunterGate(
    URL "https://github.com/FISCO-BCOS/hunter/archive/5a02ff6cd98a47538e077b3ae07e5142cf1c363a.tar.gz"
    SHA1 "0013602ad7f8d50ec39e5066f06de61f4702c434"
)

project(Tars-Service)

# install dependencies
include(${CMAKE_BINARY_DIR}/bcos-cmake-scripts/InstallBcosFrameworkDependencies.cmake)

# option(TARS_MYSQL "option for mysql" ON)
# option(TARS_SSL "option for ssl" OFF)
# option(TARS_HTTP2 "option for http2" OFF)

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

#include_directories(/home/tarsprotol/App/OtherServer)

# import tars project
add_subdirectory(protocols)
add_subdirectory(LedgerService)
add_subdirectory(StorageService)