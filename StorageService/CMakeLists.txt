cmake_minimum_required(VERSION 3.15)

project(bcostars-Storage)

add_executable(StorageService StorageServiceServer.cpp Application.cpp)
target_compile_options(StorageService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(StorageService bcostars)
target_link_libraries(StorageService initializer bcos-framework::tool bcos-storage::bcos-storage bcos-framework::table bcos-ledger::bcos-ledger tarsservant tarsutil)

add_library(StorageServiceClient StorageServiceClient.cpp)
target_compile_options(StorageServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(StorageServiceClient bcostars)
target_link_libraries(StorageServiceClient bcos-framework::utilities tarsservant tarsutil)