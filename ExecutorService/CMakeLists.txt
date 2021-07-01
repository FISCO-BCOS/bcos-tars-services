cmake_minimum_required(VERSION 3.15)

project(bcostars-Executor)

add_executable(ExecutorServiceServer ExecutorServiceServer.cpp Application.cpp)
target_compile_options(ExecutorServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(ExecutorServiceServer bcostars)
target_link_libraries(ExecutorServiceServer initializer bcos-framework::tool bcos-executor::executor bcos-ledger::bcos-ledger DispatcherServiceClient tarsservant tarsutil)

add_library(ExecutorServiceClient ExecutorServiceClient.cpp)
target_compile_options(ExecutorServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(ExecutorServiceClient bcostars)
target_link_libraries(ExecutorServiceClient bcos-framework::utilities tarsservant tarsutil)