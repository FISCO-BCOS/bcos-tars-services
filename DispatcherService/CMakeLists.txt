cmake_minimum_required(VERSION 3.15)

project(bcostars-Dispatcher)

add_executable(DispatcherService DispatcherServiceServer.cpp Application.cpp)
target_compile_options(DispatcherService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(DispatcherService bcostars)
target_link_libraries(DispatcherService initializer bcos-framework::tool bcos-framework::codec bcos-framework::protocol bcos-dispatcher::dispatcher tarsservant tarsutil)

add_library(DispatcherServiceClient DispatcherServiceClient.cpp)
target_compile_options(DispatcherServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(DispatcherServiceClient bcostars)
target_link_libraries(DispatcherServiceClient bcos-framework::utilities tarsservant tarsutil)