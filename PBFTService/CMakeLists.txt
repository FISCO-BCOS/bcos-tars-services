cmake_minimum_required(VERSION 3.15)

project(bcostars-pbft)
file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(${PBFT_INITIALIZER_LIB} PBFTInitializer.cpp)
target_compile_options(${PBFT_INITIALIZER_LIB} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${PBFT_INITIALIZER_LIB} initializer bcos-framework::tool bcos-framework::sealer bcos-pbft::bcos-pbft bcos-pbft::bcos-consensus-core bcos-sync::block-sync)

set(BINARY_NAME PBFTService)
add_executable(${BINARY_NAME} ${SRC_LIST} ${HEADERS})
target_compile_options(${BINARY_NAME} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${BINARY_NAME} ${PBFT_INITIALIZER_LIB} bcos-txpool::bcos-txpool bcos-ledger::bcos-ledger bcos-storage::bcos-storage tarscpp::tarsservant tarscpp::tarsutil)