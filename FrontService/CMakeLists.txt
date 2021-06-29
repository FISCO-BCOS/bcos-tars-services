cmake_minimum_required(VERSION 3.15)

project(bcostars-Front)

add_executable(FrontServiceServer FrontServiceServer.cpp Application.cpp)
target_compile_options(FrontServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(FrontServiceServer bcostars)
target_link_libraries(FrontServiceServer bcos-front::bcos-front tarsservant tarsutil
bcos-crypto::bcos-crypto bcos-framework::protocol bcos-framework::utilities bcos-framework::codec wedpr-crypto::crypto tbb::tbb)

add_library(FrontServiceClient FrontServiceClient.cpp)
target_compile_options(FrontServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(FrontServiceClient bcostars)
target_link_libraries(FrontServiceClient bcos-framework::utilities tarsservant tarsutil)