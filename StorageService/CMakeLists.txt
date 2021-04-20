cmake_minimum_required(VERSION 2.8)

project(bcostars-Storage)

gen_server(bcostars StorageService)

target_link_libraries(StorageService bcos-framework::protocol)