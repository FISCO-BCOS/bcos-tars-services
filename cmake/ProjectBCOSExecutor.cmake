include(FetchContent)
FetchContent_Declare(executor_project
    GIT_REPOSITORY https://${URL_BASE}/FISCO-BCOS/bcos-executor.git
    GIT_TAG        2c2c863e61c2ea630f9e96044b5f2dbe9fc752e6
    # SOURCE_DIR     ${CMAKE_SOURCE_DIR}/deps/src/bcos-executor
)

if(NOT executor_project_POPULATED)
  FetchContent_Populate(executor_project)
  list(APPEND CMAKE_MODULE_PATH ${executor_project_SOURCE_DIR}/cmake/)
  set(BUILD_SHARED_LIBS OFF)
  add_subdirectory(${executor_project_SOURCE_DIR} ${executor_project_BINARY_DIR})
endif()

add_library(bcos::executor INTERFACE IMPORTED)
set_property(TARGET bcos::executor PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${executor_project_SOURCE_DIR}/include)
set_property(TARGET bcos::executor PROPERTY INTERFACE_LINK_LIBRARIES executor)
