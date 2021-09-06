cmake_minimum_required(VERSION 3.15)

project(bcostars-rpc)

add_executable(RpcService RpcServiceServer.cpp Application.cpp)
target_compile_options(RpcService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(RpcService bcostars)
target_link_libraries(RpcService initializer bcos-rpc::rpc bcos-rpc::http bcos-rpc::jsonrpc bcos-rpc::amop bcos-rpc::wsservice tarsservant tarsutil bcos-framework::table bcos-framework::utilities bcos-ledger::bcos-ledger PBFTServiceClient TxPoolServiceClient GatewayServiceClient FrontServiceClient ExecutorServiceClient)

add_library(RpcServiceClient RpcServiceClient.cpp)
target_compile_options(RpcServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(RpcServiceClient bcostars)
target_link_libraries(RpcServiceClient bcos-framework::utilities tarsservant tarsutil)