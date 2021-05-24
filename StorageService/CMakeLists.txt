cmake_minimum_required(VERSION 3.15)

project(bcostars-Storage)

hunter_add_package(Microsoft.GSL)
hunter_add_package(tbb)
hunter_add_package(Boost COMPONENTS all)
hunter_add_package(rocksdb)
hunter_add_package(zstd)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED log thread chrono system filesystem serialization)
find_package(tbb CONFIG REQUIRED)
find_package(zstd CONFIG REQUIRED)
find_package(RocksDB CONFIG REQUIRED)

add_executable(StorageServiceServer StorageServiceServer.cpp Application.cpp)
target_compile_options(StorageServiceServer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(StorageServiceServer bcostars bcos-framework bcos-storage)
target_link_libraries(StorageServiceServer
libbcos-storage.a libtabled.a libutilitiesd.a zstd::libzstd_static Threads::Threads tarsservant tarsutil RocksDB::rocksdb Microsoft.GSL::GSL Boost::serialization Boost::log Boost::thread Boost::chrono Boost::filesystem tbb)

add_library(StorageServiceClient StorageServiceClient.cpp)
target_compile_options(StorageServiceClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(StorageServiceClient bcostars bcos-framework bcos-storage)
target_link_libraries(StorageServiceClient tarsservant tarsutil Microsoft.GSL::GSL)