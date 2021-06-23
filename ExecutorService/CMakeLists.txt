cmake_minimum_required(VERSION 3.15)

project(bcostars-Executor)

hunter_add_package(Microsoft.GSL)
hunter_add_package(tbb)
hunter_add_package(Boost COMPONENTS all)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED log thread chrono system filesystem serialization)
find_package(tbb CONFIG REQUIRED)

add_executable(ExecutorServiceServer ExecutorServiceServer.cpp Application.cpp)
target_compile_options(ExecutorServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(ExecutorServiceServer bcostars bcos-framework bcos-executor bcos-crypto bcos-ledger)
target_link_libraries(ExecutorServiceServer
libexecutor.a libprecompiled.a libstate.a libvm.a libutilitiesd.a tarsservant tarsutil Microsoft.GSL::GSL Boost::serialization Boost::log Boost::thread Boost::chrono Boost::filesystem tbb::tbb)

add_library(ExecutorServiceClient ExecutorServiceClient.cpp)
target_compile_options(ExecutorServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(ExecutorServiceClient bcostars bcos-framework)
target_link_libraries(ExecutorServiceClient tarsservant tarsutil Microsoft.GSL::GSL)