cmake_minimum_required(VERSION 3.15)

project(bcostars-Front)

add_executable(FrontService FrontServiceServer.cpp Application.cpp)
target_compile_options(FrontService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(FrontService initializer bcos-framework::tool bcos-front::bcos-front PBFTServiceClient GatewayServiceClient tarscpp::tarsservant tarscpp::tarsutil)

add_library(FrontServiceClient FrontServiceClient.cpp)
target_compile_options(FrontServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(FrontServiceClient bcos-framework::utilities tarscpp::tarsservant tarscpp::tarsutil)