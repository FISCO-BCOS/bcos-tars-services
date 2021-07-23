cmake_minimum_required(VERSION 3.15)

project(bcostars-rpc)

add_executable(RpcService RpcServiceServer.cpp Application.cpp)
target_compile_options(RpcService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(RpcService bcostars)
target_link_libraries(RpcService PBFTServiceClient initializer bcos-framework::tool bcos-framework::table bcos-txpool::bcos-txpool bcos-ledger::bcos-ledger bcos-framework::utilities tarsservant tarsutil OpenSSL::SSL OpenSSL::Crypto)

add_library(RpcServiceClient RpcServiceClient.cpp)
target_compile_options(RpcServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(RpcServiceClient bcostars)
target_link_libraries(RpcServiceClient bcos-framework::utilities tarsservant tarsutil)