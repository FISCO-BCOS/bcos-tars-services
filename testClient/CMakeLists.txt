cmake_minimum_required(VERSION 3.15)

project(testClient)

file(GLOB_RECURSE SOURCES "*.cpp")

add_executable(testClient ${SOURCES})
add_dependencies(testClient LedgerService)
target_compile_options(testClient PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(testClient tarsservant tarsutil)