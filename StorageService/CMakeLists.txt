cmake_minimum_required(VERSION 3.15)

project(bcostars-Storage)

# gen_server(bcostars StorageService)
FILE(GLOB_RECURSE SRC_FILES  "*.cpp")
add_executable(StorageService ${SRC_FILES})
target_compile_options(StorageService PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)

find_package(Microsoft.GSL CONFIG REQUIRED)
find_package(Boost CONFIG REQUIRED log thread chrono system filesystem)
find_package(tbb CONFIG REQUIRED)
add_dependencies(StorageService bcostars bcos-framework bcos-storage)
target_link_libraries(StorageService
libbcos-storage.a libtabled.a libutilitiesd.a tarsservant tarsutil Microsoft.GSL::GSL Boost::log Boost::thread Boost::chrono Boost::filesystem tbb)