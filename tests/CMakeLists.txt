cmake_minimum_required(VERSION 3.15)

project(Tars-Service-Test)

file(GLOB_RECURSE SOURCES "*.cpp")

hunter_add_package(wedpr-crypto)
hunter_add_package(tbb)
hunter_add_package(Boost COMPONENTS all)

find_package(wedpr-crypto CONFIG QUIET REQUIRED)
find_package(Boost CONFIG REQUIRED unit_test_framework thread)
find_package(tbb CONFIG REQUIRED)

add_executable(bcos-tars-service-test ${SOURCES})
add_dependencies(bcos-tars-service-test bcos-framework bcostars)
target_compile_options(bcos-tars-service-test PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(bcos-tars-service-test libbcos-cryptod.a libprotocold.a libcodecd.a wedpr-crypto::crypto Boost::unit_test_framework Boost::thread crypto tbb::tbb)

add_test(NAME tars-test WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} COMMAND bcos-tars-service-test)