cmake_minimum_required(VERSION 3.15)

project(Tars-Service-Test)

file(GLOB_RECURSE SOURCES "*.cpp")

hunter_add_package(wedpr-crypto)
find_package(wedpr-crypto CONFIG QUIET REQUIRED)
find_package(Boost CONFIG REQUIRED unit_test_framework thread)

add_executable(bcos-tars-service-test ${SOURCES})
add_dependencies(bcos-tars-service-test bcos-framework bcostars)
target_compile_options(bcos-tars-service-test PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(bcos-tars-service-test Boost::unit_test_framework Boost::thread wedpr-crypto::crypto crypto)

add_test(NAME tars-test WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND bcos-tars-service-test)