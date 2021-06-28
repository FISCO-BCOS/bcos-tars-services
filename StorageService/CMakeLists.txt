cmake_minimum_required(VERSION 3.15)

project(bcostars-Storage)

add_executable(StorageServiceServer StorageServiceServer.cpp Application.cpp)
target_compile_options(StorageServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(StorageServiceServer bcostars)
target_link_libraries(StorageServiceServer bcos-storage::bcos-storage bcos-framework::table Threads::Threads tarsservant tarsutil)

add_library(StorageServiceClient StorageServiceClient.cpp)
target_compile_options(StorageServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(StorageServiceClient bcostars)
target_link_libraries(StorageServiceClient bcos-framework::utilities tarsservant tarsutil)