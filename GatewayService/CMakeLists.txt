cmake_minimum_required(VERSION 3.15)

project(bcostars-Gateway)

add_executable(GatewayServiceServer GatewayServiceServer.cpp Application.cpp)
target_compile_options(GatewayServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(GatewayServiceServer bcostars)
target_link_libraries(GatewayServiceServer bcos-gateway::bcos-gateway bcos-framework::utilities bcos-gateway::network bcos-gateway::p2p tarsservant tarsutil OpenSSL::SSL OpenSSL::Crypto)

add_library(GatewayServiceClient GatewayServiceClient.cpp)
target_compile_options(GatewayServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(GatewayServiceClient bcostars)
target_link_libraries(GatewayServiceClient bcos-framework::utilities tarsservant tarsutil)