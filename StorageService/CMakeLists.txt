cmake_minimum_required(VERSION 2.8)

project(bcostars-Storage)

gen_server(bcostars StorageService)
target_compile_options(StorageService PRIVATE -Wno-error)

add_dependencies(StorageService bcos-framework)