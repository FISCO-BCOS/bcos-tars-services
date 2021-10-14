cmake_minimum_required(VERSION 3.15)

project(bcostars-group-manager)

file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

set(BINARY_NAME GroupManagerService)
add_executable(${BINARY_NAME} ${SRC_LIST} ${HEADERS})
target_compile_options(${BINARY_NAME} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${BINARY_NAME} initializer tarscpp::tarsservant tarscpp::tarsutil bcos-framework::utilities bcos-group-manager::group-manager)