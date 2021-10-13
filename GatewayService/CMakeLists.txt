cmake_minimum_required(VERSION 3.15)

project(bcostars-Gateway)

add_executable(GatewayService GatewayServiceServer.cpp Application.cpp)
target_compile_options(GatewayService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(GatewayService initializer bcos-framework::tool bcos-gateway::bcos-gateway bcos-framework::utilities bcos-gateway::network bcos-gateway::p2p tarscpp::tarsservant tarscpp::tarsutil OpenSSL::SSL OpenSSL::Crypto)