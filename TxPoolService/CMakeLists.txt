cmake_minimum_required(VERSION 3.15)

project(bcostars-txpool)
file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(${TXPOOL_INITIALIZER_LIB} TxPoolInitializer.cpp)
target_compile_options(${TXPOOL_INITIALIZER_LIB} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${TXPOOL_INITIALIZER_LIB} initializer bcos-framework::tool bcos-txpool::bcos-txpool)


set(BINARY_NAME TxPoolService)
add_executable(${BINARY_NAME} ${SRC_LIST} ${HEADERS})
target_compile_options(${BINARY_NAME} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${BINARY_NAME} ${TXPOOL_INITIALIZER_LIB} bcos-framework::storage bcos-ledger::bcos-ledger bcos-framework::utilities tarscpp::tarsservant tarscpp::tarsutil OpenSSL::SSL OpenSSL::Crypto)