file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

hunter_add_package(bcos-boostssl)
find_package(bcos-boostssl CONFIG REQUIRED)

add_executable(fisco-bcos ${SRC_LIST} ${HEADERS})

list(APPEND InitLibs ${FRONT_INITIALIZER_LIB} ${TXPOOL_INITIALIZER_LIB} ${PBFT_INITIALIZER_LIB} bcos-framework::utilities bcos-framework::tool bcos-framework::storage
OpenSSL::SSL OpenSSL::Crypto bcos-framework::protocol-pb bcos-framework::sealer bcos-tars-protocol::protocol-tars bcos-crypto::bcos-crypto 
bcos-storage::bcos-storage bcos-ledger::bcos-ledger bcos-scheduler::scheduler bcos::executor)

# target_compile_options(fisco-bcos PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)

target_link_libraries(fisco-bcos PUBLIC ${InitLibs})