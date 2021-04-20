cmake_minimum_required(VERSION 3.15)

set(GIT_URL_BASE "github.com.cnpmjs.org")

include(FetchContent)
FetchContent_Declare(bcos-cmake-scripts
GIT_REPOSITORY https://${GIT_URL_BASE}/FISCO-BCOS/bcos-cmake-scripts.git
GIT_TAG dev
SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/bcos-cmake-scripts
)
FetchContent_MakeAvailable(bcos-cmake-scripts)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/bcos-cmake-scripts)

FetchContent_Declare(bcos-framework
GIT_REPOSITORY https://${GIT_URL_BASE}/FISCO-BCOS/bcos-framework.git
GIT_TAG dev
SOURCE_DIR ${CMAKE_CURRENT_BINARY_DIR}/bcos-framework
)
FetchContent_MakeAvailable(bcos-framework)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/bcos-framework)