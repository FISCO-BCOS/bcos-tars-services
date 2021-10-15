cmake_minimum_required(VERSION 3.15)

project(bcostars-Gateway)

file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

set(BINARY_NAME GatewayService)
add_executable(${BINARY_NAME} ${SRC_LIST} ${HEADERS})

target_compile_options(${BINARY_NAME} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${BINARY_NAME} initializer bcos-framework::tool bcos-gateway::bcos-gateway bcos-framework::utilities bcos-gateway::network bcos-gateway::p2p tarscpp::tarsservant tarscpp::tarsutil OpenSSL::SSL OpenSSL::Crypto)