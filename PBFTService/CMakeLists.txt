cmake_minimum_required(VERSION 3.15)

project(bcostars-PBFT)

hunter_add_package(Microsoft.GSL)
hunter_add_package(tbb)
hunter_add_package(Boost COMPONENTS all)
hunter_add_package(wedpr-crypto)
hunter_add_package(Protobuf)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED log thread chrono system filesystem serialization)
find_package(tbb CONFIG REQUIRED)
find_package(wedpr-crypto CONFIG QUIET REQUIRED)
find_package(Protobuf CONFIG QUIET REQUIRED)

add_executable(PBFTServiceServer PBFTServiceServer.cpp Application.cpp)
target_compile_options(PBFTServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(PBFTServiceServer bcostars bcos-framework bcos-front)

target_link_libraries(PBFTServiceServer libsealerd.a libbcos-pbftd.a libbcos-consensus-cored.a libblock-syncd.a
libbcos-txpoold.a libsyncd.a libbcos-frontd.a libbcos-ledgerd.a libbcos-storage.a
libtoold.a libtabled.a libprotocol-pbd.a libprotocold.a libcodecd.a libbcos-cryptod.a libutilitiesd.a wedpr-crypto::crypto
tarsservant tarsutil
protobuf::libprotobuf Microsoft.GSL::GSL Boost::serialization Boost::log Boost::thread Boost::chrono Boost::filesystem tbb::tbb
)

add_library(PBFTServiceClient PBFTServiceClient.cpp)
target_compile_options(PBFTServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(PBFTServiceClient bcostars bcos-framework bcos-front)
target_link_libraries(PBFTServiceClient tarsservant tarsutil Microsoft.GSL::GSL)