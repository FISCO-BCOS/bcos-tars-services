cmake_minimum_required(VERSION 3.15)

project(bcostars-LedgerService)

gen_server(bcostars LedgerService)
target_compile_options(LedgerService PRIVATE -Wno-error)

add_dependencies(LedgerService bcos-framework)