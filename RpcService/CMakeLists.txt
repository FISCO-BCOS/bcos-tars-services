cmake_minimum_required(VERSION 3.15)

project(bcostars-rpc)
file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

set(BINARY_NAME RpcService)
add_executable(${BINARY_NAME} ${SRC_LIST} ${HEADERS})
find_package(bcos-boostssl)
target_compile_options(${BINARY_NAME} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${BINARY_NAME} initializer bcos-rpc::rpc bcos-boostssl::boostssl-context bcos-boostssl::boostssl-websocket bcos-boostssl::boostssl-httpserver bcos-rpc::jsonrpc bcos-rpc::amop tarscpp::tarsservant tarscpp::tarsutil bcos-framework::storage bcos-framework::utilities bcos-ledger::bcos-ledger)