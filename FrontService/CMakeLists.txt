cmake_minimum_required(VERSION 3.15)

project(bcostars-Front)

hunter_add_package(Microsoft.GSL)
hunter_add_package(tbb)
hunter_add_package(Boost COMPONENTS all)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED log thread chrono system filesystem serialization)
find_package(tbb CONFIG REQUIRED)

add_executable(FrontServiceServer FrontServiceServer.cpp Application.cpp)
target_compile_options(FrontServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(FrontServiceServer bcostars bcos-framework bcos-storage)
target_link_libraries(FrontServiceServer
libbcos-front.a tarsservant tarsutil Microsoft.GSL::GSL Boost::serialization Boost::log Boost::thread Boost::chrono Boost::filesystem tbb::tbb)

add_library(StorageServiceClient StorageServiceClient.cpp)
target_compile_options(StorageServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(StorageServiceClient bcostars bcos-framework bcos-storage)
target_link_libraries(StorageServiceClient tarsservant tarsutil Microsoft.GSL::GSL)