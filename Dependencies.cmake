cmake_minimum_required(VERSION 3.15)

if (NOT DEFINED GIT_URL_BASE)
    set(GIT_URL_BASE "github.com")
   endif()
# set(GIT_URL_BASE "github.com.cnpmjs.org")
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
ExternalProject_Add(tarscpp
    GIT_REPOSITORY https://${GIT_URL_BASE}/TarsCloud/TarsCpp.git
    GIT_TAG release/2.4
    SOURCE_DIR ${DEPENDENCIES_DIR}/tarscpp
    CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_DIR}/tarscpp-install
    PATCH_COMMAND bash ${CMAKE_CURRENT_SOURCE_DIR}/patchTars.sh ${DEPENDENCIES_DIR}/tarscpp ${DEPENDENCIES_DIR}/tarscpp-install
)
include_directories(${DEPENDENCIES_DIR}/tarscpp-install/include)
link_directories(${DEPENDENCIES_DIR}/tarscpp-install/lib)
set(TARS2CPP ${DEPENDENCIES_DIR}/tarscpp-install/tools/tars2cpp)

foreach(BCOS_MODULE framework front storage dispatcher ledger gateway pbft sync txpool executor crypto)
    ExternalProject_Add(bcos-${BCOS_MODULE}
        GIT_REPOSITORY https://${GIT_URL_BASE}/FISCO-BCOS/bcos-${BCOS_MODULE}.git
        GIT_TAG dev
        SOURCE_DIR ${DEPENDENCIES_DIR}/bcos-${BCOS_MODULE}
        CMAKE_ARGS -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} -DURL_BASE=${GIT_URL_BASE} -DCMAKE_INSTALL_PREFIX=${DEPENDENCIES_DIR}/bcos-${BCOS_MODULE}-install
    )
    include_directories(${DEPENDENCIES_DIR}/bcos-${BCOS_MODULE}-install/include)
    link_directories(${DEPENDENCIES_DIR}/bcos-${BCOS_MODULE}-install/lib)
endforeach(BCOS_MODULE)