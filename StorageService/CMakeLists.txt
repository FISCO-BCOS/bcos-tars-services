cmake_minimum_required(VERSION 2.8)

project(bcostars-Storage)

gen_server(bcostars StorageService)
target_compile_options(StorageService PRIVATE -Wno-error)

find_package(Microsoft.GSL CONFIG REQUIRED)
add_dependencies(StorageService bcos-framework)
target_link_libraries(StorageService Microsoft.GSL::GSL)