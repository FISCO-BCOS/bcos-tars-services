cmake_minimum_required(VERSION 3.15)

project(bcostars-txpool)
file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(${TXPOOL_SERVICE_LIB} ${SRC_LIST})
target_compile_options(${TXPOOL_SERVICE_LIB} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${TXPOOL_SERVICE_LIB} bcos-tars-protocol::protocol-tars initializer bcos-framework::tool bcos-txpool::bcos-txpool tarscpp::tarsservant tarscpp::tarsutil)


set(BINARY_NAME TxPoolService)
add_executable(${BINARY_NAME} ${SRC_LIST} ${HEADERS})
target_compile_options(${BINARY_NAME} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${BINARY_NAME} ${TXPOOL_SERVICE_LIB} bcos-framework::storage bcos-ledger::bcos-ledger bcos-framework::utilities)