cmake_minimum_required(VERSION 3.15)

project(bcostars-Gateway)

hunter_add_package(Microsoft.GSL)
hunter_add_package(tbb)
hunter_add_package(Boost COMPONENTS all)
hunter_add_package(OpenSSL)
hunter_add_package(jsoncpp)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED log thread chrono system filesystem serialization)
find_package(tbb CONFIG REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(jsoncpp CONFIG REQUIRED)

add_executable(GatewayServiceServer GatewayServiceServer.cpp Application.cpp)
target_compile_options(GatewayServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(GatewayServiceServer bcostars bcos-framework bcos-gateway)
target_link_libraries(GatewayServiceServer
libbcos-gatewayd.a libnetworkd.a libp2pd.a libutilitiesd.a jsoncpp_lib_static
tarsservant tarsutil OpenSSL::SSL OpenSSL::Crypto Microsoft.GSL::GSL Boost::serialization Boost::log Boost::thread Boost::chrono Boost::filesystem tbb::tbb)

add_library(GatewayServiceClient GatewayServiceClient.cpp)
target_compile_options(GatewayServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(GatewayServiceClient bcostars bcos-framework bcos-front)
target_link_libraries(GatewayServiceClient tarsservant tarsutil Microsoft.GSL::GSL)