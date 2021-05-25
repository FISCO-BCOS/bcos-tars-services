cmake_minimum_required(VERSION 3.15)

project(bcostars-testClient)

hunter_add_package(Microsoft.GSL)
hunter_add_package(tbb)
hunter_add_package(Boost COMPONENTS all)
hunter_add_package(rocksdb)
hunter_add_package(zstd)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED log thread chrono system filesystem serialization program_options)
find_package(zstd CONFIG REQUIRED)
find_package(tbb CONFIG REQUIRED)
find_package(RocksDB CONFIG REQUIRED)

add_executable(StorageServiceTestClient StorageServiceTestClient.cpp)
target_compile_options(StorageServiceTestClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(StorageServiceTestClient StorageServiceClient)
target_link_libraries(StorageServiceTestClient libbcos-storage.a libtabled.a libutilitiesd.a zstd::libzstd_static Threads::Threads tarsservant tarsutil RocksDB::rocksdb zstd::libzstd_static Microsoft.GSL::GSL Boost::serialization Boost::log Boost::thread Boost::chrono Boost::filesystem Boost::program_options tbb::tbb)