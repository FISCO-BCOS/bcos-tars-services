cmake_minimum_required(VERSION 3.15)

project(bcostars-Dispatcher)

add_executable(DispatcherServiceServer DispatcherServiceServer.cpp Application.cpp)
target_compile_options(DispatcherServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(DispatcherServiceServer bcostars)
target_link_libraries(DispatcherServiceServer bcos-framework::codec bcos-framework::protocol bcos-dispatcher::dispatcher bcos-crypto::bcos-crypto tbb::tbb tarsservant tarsutil)

add_library(DispatcherServiceClient DispatcherServiceClient.cpp)
target_compile_options(DispatcherServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(DispatcherServiceClient bcostars)
target_link_libraries(DispatcherServiceClient bcos-framework::utilities tarsservant tarsutil)