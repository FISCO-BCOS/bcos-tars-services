cmake_minimum_required(VERSION 3.15)

project(bcostars-front)
file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")


add_library(${FRONT_INITIALIZER_LIB} FrontServiceInitializer.cpp)
target_compile_options(${FRONT_INITIALIZER_LIB} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${FRONT_INITIALIZER_LIB} initializer bcos-framework::tool bcos-front::bcos-front)

set(BINARY_NAME FrontService)
add_executable(${BINARY_NAME} ${SRC_LIST} ${HEADERS})
target_compile_options(${BINARY_NAME} PRIVATE -Wno-error -Wno-unused-parameter -Wno-variadic-macros -Wno-return-type -Wno-pedantic)
target_link_libraries(${BINARY_NAME} ${FRONT_INITIALIZER_LIB} tarscpp::tarsservant tarscpp::tarsutil)