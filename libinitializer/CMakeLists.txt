file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(initializer ${SRC_LIST} ${HEADERS})
target_compile_options(initializer PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
add_dependencies(initializer bcostars)
target_link_libraries(initializer PUBLIC Boost::program_options bcos-framework::utilities bcos-framework::codec bcos-framework::protocol bcos-framework::tool OpenSSL::SSL OpenSSL::Crypto 
                     bcos-framework::protocol bcos-crypto::bcos-crypto)
