cmake_minimum_required(VERSION 3.15)

set(GIT_URL_BASE "github.com.cnpmjs.org")
set (DEPENDENCIES_DIR ${CMAKE_CURRENT_BINARY_DIR})

include(FetchContent)
FetchContent_Declare(bcos-cmake-scripts
GIT_REPOSITORY https://${GIT_URL_BASE}/FISCO-BCOS/bcos-cmake-scripts.git
GIT_TAG dev
SOURCE_DIR ${DEPENDENCIES_DIR}/bcos-cmake-scripts
)
FetchContent_MakeAvailable(bcos-cmake-scripts)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_BINARY_DIR}/bcos-cmake-scripts)

include(ExternalProject)
# foreach(BCOS_MODULE framework ledger storage)
foreach(BCOS_MODULE framework)
    ExternalProject_Add(bcos-${BCOS_MODULE}
        GIT_REPOSITORY https://${GIT_URL_BASE}/FISCO-BCOS/bcos-${BCOS_MODULE}.git
        GIT_TAG dev
        SOURCE_DIR ${DEPENDENCIES_DIR}/bcos-${BCOS_MODULE}
        CMAKE_ARGS -DURL_BASE=${GIT_URL_BASE} -DFETCH_URL_BASE=${GIT_URL_BASE} -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_DIR}/bcos-${BCOS_MODULE}-install
    )
    include_directories(${DEPENDENCIES_DIR}/bcos-${BCOS_MODULE}-install/include)
    link_directories(${DEPENDENCIES_DIR}/bcos-${BCOS_MODULE}-install/lib)
endforeach(BCOS_MODULE)