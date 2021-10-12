file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_executable(fisco-bcos ${SRC_LIST} ${HEADERS})

list(APPEND InitLibs bcos-gateway::bcos-gateway bcos-framework::utilities bcos-framework::tool bcos-framework::storage
OpenSSL::SSL OpenSSL::Crypto bcos-framework::protocol-pb bcos-framework::sealer bcos-tars-protocol::protocol-tars bcos-crypto::bcos-crypto
bcos-front::bcos-front bcos-storage::bcos-storage bcos-txpool::bcos-txpool bcos-pbft::bcos-pbft
bcos-sync::block-sync bcos-ledger::bcos-ledger bcos-scheduler::scheduler bcos::executor bcos-rpc::rpc bcos-rpc::jsonrpc bcos-rpc::amop)

# target_compile_options(fisco-bcos PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)

target_link_libraries(fisco-bcos PUBLIC ${InitLibs})