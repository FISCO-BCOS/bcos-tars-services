cmake_minimum_required(VERSION 3.15)

project(bcostars-TxPool)

add_executable(TxPoolService TxPoolServiceServer.cpp Application.cpp)
target_compile_options(TxPoolService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(TxPoolService initializer bcos-framework::tool bcos-framework::storage bcos-txpool::bcos-txpool bcos-ledger::bcos-ledger bcos-framework::utilities tarscpp::tarsservant tarscpp::tarsutil OpenSSL::SSL OpenSSL::Crypto)