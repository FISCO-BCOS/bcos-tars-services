cmake_minimum_required(VERSION 3.15)

project(bcostars-TxPool)

add_executable(TxPoolServiceServer TxPoolServiceServer.cpp Application.cpp)
target_compile_options(TxPoolServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(TxPoolServiceServer bcostars)
target_link_libraries(TxPoolServiceServer bcos-txpool::bcos-txpool bcos-framework::utilities tarsservant tarsutil OpenSSL::SSL OpenSSL::Crypto)

add_library(TxPoolServiceClient TxPoolServiceClient.cpp)
target_compile_options(TxPoolServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(TxPoolServiceClient bcostars)
target_link_libraries(TxPoolServiceClient bcos-framework::utilities tarsservant tarsutil)