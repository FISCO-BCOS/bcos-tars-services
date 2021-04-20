cmake_minimum_required(VERSION 3.15)

# import dependencies
include (Dependencies.cmake)

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

# import tars project
add_subdirectory(tars)
add_subdirectory(protocols)
add_subdirectory(LedgerService)
add_subdirectory(StorageService)