cmake_minimum_required(VERSION 3.15)

project(bcostars-rpc)

find_package(bcos-boostssl)
add_executable(RpcService RpcServiceServer.cpp Application.cpp)
target_compile_options(RpcService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(RpcService initializer bcos-rpc::rpc bcos-boostssl::boostssl-context bcos-boostssl::boostssl-websocket bcos-boostssl::boostssl-httpserver bcos-rpc::jsonrpc bcos-rpc::amop tarscpp::tarsservant tarscpp::tarsutil bcos-framework::storage bcos-framework::utilities bcos-ledger::bcos-ledger)