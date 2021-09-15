cmake_minimum_required(VERSION 3.15)

project(bcostars-testClient)
find_package(zstd CONFIG REQUIRED)
find_package(RocksDB CONFIG REQUIRED)
find_package(Boost CONFIG QUIET REQUIRED program_options)

add_executable(StorageServiceTestClient StorageServiceTestClient.cpp)
target_compile_options(StorageServiceTestClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(StorageServiceTestClient bcos-storage::bcos-storage bcos-framework::table Threads::Threads tarscpp::tarsservant tarscpp::tarsutil Boost::program_options)

add_executable(TransactionTestClient TransactionTestClient.cpp)
target_compile_options(TransactionTestClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(TransactionTestClient bcos-ledger::bcos-ledger bcos-framework::table bcos-framework::tool bcos-framework::codec bcos-framework::protocol bcos-crypto::bcos-crypto tarscpp::tarsservant tarscpp::tarsutil bcos-tars-protocol::protocol-tars Boost::program_options)