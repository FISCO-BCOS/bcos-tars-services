cmake_minimum_required(VERSION 3.15)

project(bcostars-Storage)

# gen_server(bcostars StorageService)
FILE(GLOB_RECURSE SRC_FILES  "*.cpp")
add_executable(StorageService ${SRC_FILES})
target_compile_options(StorageService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)

find_package(Microsoft.GSL CONFIG REQUIRED)
add_dependencies(StorageService bcostars bcos-framework)
target_link_libraries(StorageService Microsoft.GSL::GSL tarsservant tarsutil)