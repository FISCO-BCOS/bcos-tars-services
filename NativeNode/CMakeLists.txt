file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

set(BINARY_NAME NativeNode)
add_executable(${BINARY_NAME} ${SRC_LIST} ${HEADERS})

list(APPEND InitLibs ${FRONT_SERVICE_LIB} ${TXPOOL_SERVICE_LIB} ${PBFT_SERVICE_LIB} ${SCHEDULER_SERVICE_LIB} ${LEDGER_SERVICE_LIB} bcos-framework::utilities bcos-framework::tool bcos-framework::storage
OpenSSL::SSL OpenSSL::Crypto bcos-framework::protocol-pb bcos-framework::sealer bcos-tars-protocol::protocol-tars bcos-crypto::bcos-crypto 
bcos-storage::bcos-storage bcos-ledger::bcos-ledger bcos-scheduler::scheduler bcos::executor tarscpp::tarsservant tarscpp::tarsutil)

# target_compile_options(${BINARY_NAME} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)

target_link_libraries(${BINARY_NAME} PUBLIC ${InitLibs})