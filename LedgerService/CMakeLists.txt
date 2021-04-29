cmake_minimum_required(VERSION 3.15)

project(bcostars-LedgerService)

gen_server(bcostars LedgerService)
target_compile_options(LedgerService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)

find_package(Microsoft.GSL CONFIG REQUIRED)
add_dependencies(LedgerService bcostars bcos-framework)
target_link_libraries(LedgerService Microsoft.GSL::GSL)