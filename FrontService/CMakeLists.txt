cmake_minimum_required(VERSION 3.15)

project(bcostars-Front)

add_executable(FrontServiceServer FrontServiceServer.cpp Application.cpp)
target_compile_options(FrontServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(FrontServiceServer bcostars)
target_link_libraries(FrontServiceServer initializer bcos-framework::tool bcos-front::bcos-front PBFTServiceClient GatewayServiceClient tarsservant tarsutil)

add_library(FrontServiceClient FrontServiceClient.cpp)
target_compile_options(FrontServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(FrontServiceClient bcostars)
target_link_libraries(FrontServiceClient bcos-framework::utilities tarsservant tarsutil)