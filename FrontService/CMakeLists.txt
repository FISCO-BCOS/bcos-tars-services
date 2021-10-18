cmake_minimum_required(VERSION 3.15)

project(bcostars-front)
file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")


add_library(${FRONT_SERVICE_LIB} ${SRC_LIST})
target_compile_options(${FRONT_SERVICE_LIB} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${FRONT_SERVICE_LIB} bcos-tars-protocol::protocol-tars initializer bcos-framework::tool bcos-front::bcos-front tarscpp::tarsservant tarscpp::tarsutil)

set(BINARY_NAME FrontService)
add_executable(${BINARY_NAME} ${SRC_LIST} ${HEADERS})
target_compile_options(${BINARY_NAME} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${BINARY_NAME} ${FRONT_SERVICE_LIB})