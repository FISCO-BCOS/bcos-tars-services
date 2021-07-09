cmake_minimum_required(VERSION 3.15)

project(bcostars-Executor)

add_executable(ExecutorService ExecutorServiceServer.cpp Application.cpp)
target_compile_options(ExecutorService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(ExecutorService bcostars)
target_link_libraries(ExecutorService initializer bcos-framework::tool bcos::executor bcos-ledger::bcos-ledger DispatcherServiceClient tarsservant tarsutil)

add_library(ExecutorServiceClient ExecutorServiceClient.cpp)
target_compile_options(ExecutorServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(ExecutorServiceClient bcostars)
target_link_libraries(ExecutorServiceClient bcos-framework::utilities tarsservant tarsutil)