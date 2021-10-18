cmake_minimum_required(VERSION 3.15)

project(bcostars-pbft)
file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(${PBFT_SERVICE_LIB} ${SRC_LIST})
target_compile_options(${PBFT_SERVICE_LIB} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${PBFT_SERVICE_LIB} bcos-ledger::bcos-ledger bcos-tars-protocol::protocol-tars initializer bcos-framework::tool bcos-framework::sealer bcos-pbft::bcos-pbft bcos-pbft::bcos-consensus-core bcos-sync::block-sync tarscpp::tarsservant tarscpp::tarsutil)

set(BINARY_NAME PBFTService)
add_executable(${BINARY_NAME} ${SRC_LIST} ${HEADERS})
target_compile_options(${BINARY_NAME} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${BINARY_NAME} ${PBFT_SERVICE_LIB} bcos-storage::bcos-storage)