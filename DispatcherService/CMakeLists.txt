cmake_minimum_required(VERSION 3.15)

project(bcostars-Dispatcher)

add_executable(DispatcherService DispatcherServiceServer.cpp Application.cpp)
target_compile_options(DispatcherService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(DispatcherService initializer bcos-framework::tool bcos-framework::codec bcos-framework::protocol bcos-scheduler::scheduler tarscpp::tarsservant tarscpp::tarsutil)

add_library(DispatcherServiceClient DispatcherServiceClient.cpp)
target_compile_options(DispatcherServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(DispatcherServiceClient bcos-framework::utilities tarscpp::tarsservant tarscpp::tarsutil)