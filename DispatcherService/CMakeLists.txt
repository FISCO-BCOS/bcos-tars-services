cmake_minimum_required(VERSION 3.15)

project(bcostars-Dispatcher)

hunter_add_package(Microsoft.GSL)
hunter_add_package(tbb)
hunter_add_package(Boost COMPONENTS all)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED log thread chrono system filesystem serialization)
find_package(tbb CONFIG REQUIRED)
find_package(wedpr-crypto CONFIG QUIET REQUIRED)

add_executable(DispatcherServiceServer DispatcherServiceServer.cpp Application.cpp)
target_compile_options(DispatcherServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(DispatcherServiceServer bcostars bcos-framework bcos-dispatcher)
target_link_libraries(DispatcherServiceServer
libdispatcherd.a libprotocol.a libcodecd.a libbcos-cryptod.a libutilitiesd.a wedpr-crypto::crypto tarsservant tarsutil Microsoft.GSL::GSL Boost::serialization Boost::log Boost::thread Boost::chrono Boost::filesystem tbb::tbb)

add_library(DispatcherServiceClient DispatcherServiceClient.cpp)
target_compile_options(DispatcherServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(DispatcherServiceClient bcostars bcos-framework)
target_link_libraries(DispatcherServiceClient tarsservant tarsutil Microsoft.GSL::GSL)