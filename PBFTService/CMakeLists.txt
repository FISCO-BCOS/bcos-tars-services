cmake_minimum_required(VERSION 3.15)

project(bcostars-PBFT)
file(GLOB HEADERS "*.h")

add_executable(PBFTService PBFTServiceServer.cpp Application.cpp ${HEADERS})
target_compile_options(PBFTService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(PBFTService bcostars)
target_link_libraries(PBFTService initializer bcos-framework::tool bcos-framework::sealer bcos-pbft::bcos-pbft bcos-pbft::bcos-consensus-core bcos-sync::block-sync bcos-txpool::bcos-txpool
bcos-front::bcos-front bcos-ledger::bcos-ledger bcos-storage::bcos-storage bcos-framework::tool bcos-crypto::bcos-crypto tarsservant tarsutil)

add_library(PBFTServiceClient PBFTServiceClient.cpp ${HEADERS})
target_compile_options(PBFTServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(PBFTServiceClient bcostars)
target_link_libraries(PBFTServiceClient bcos-framework::utilities tarsservant tarsutil)