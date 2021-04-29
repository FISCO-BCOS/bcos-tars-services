cmake_minimum_required(VERSION 3.15)

project(Tars-Service-Test)

file(GLOB_RECURSE SOURCES "*.cpp")

find_package(Boost CONFIG REQUIRED unit_test_framework)

add_executable(bcos-tars-service-test ${SOURCES})
target_compile_options(bcos-tars-service-test PRIVATE -Wno-error)
target_link_libraries(bcos-tars-service-test Boost::unit_test_framework)