cmake_minimum_required(VERSION 3.15)

project(bcostars-TxPool)

add_executable(TxPoolService TxPoolServiceServer.cpp Application.cpp)
target_compile_options(TxPoolService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(TxPoolService PBFTServiceClient initializer bcos-framework::tool bcos-framework::table bcos-txpool::bcos-txpool bcos-ledger::bcos-ledger bcos-framework::utilities tarscpp::tarsservant tarscpp::tarsutil OpenSSL::SSL OpenSSL::Crypto)

add_library(TxPoolServiceClient TxPoolServiceClient.cpp)
target_compile_options(TxPoolServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(TxPoolServiceClient bcos-framework::utilities tarscpp::tarsservant tarscpp::tarsutil)