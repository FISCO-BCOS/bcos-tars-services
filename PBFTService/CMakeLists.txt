cmake_minimum_required(VERSION 3.15)

project(bcostars-PBFT)

hunter_add_package(Microsoft.GSL)
hunter_add_package(tbb)
hunter_add_package(Boost COMPONENTS all)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED log thread chrono system filesystem serialization)
find_package(tbb CONFIG REQUIRED)

add_executable(PBFTServiceServer PBFTServiceServer.cpp Application.cpp)
target_compile_options(PBFTServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(PBFTServiceServer bcostars bcos-framework bcos-storage bcos-front)
target_link_libraries(PBFTServiceServer
libbcos-frontd.a libbcos-storage.a libtabled.a libutilitiesd.a tarsservant tarsutil Microsoft.GSL::GSL Boost::serialization Boost::log Boost::thread Boost::chrono Boost::filesystem tbb::tbb)

# add_library(PBFTServiceClient PBFTServiceClient.cpp)
# target_compile_options(PBFTServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
# add_dependencies(PBFTServiceClient bcostars bcos-framework bcos-front)
# target_link_libraries(PBFTServiceClient tarsservant tarsutil Microsoft.GSL::GSL)