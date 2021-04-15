cmake_minimum_required(VERSION 2.8)

project(bcos_tars-Storage)

gen_server(bcos_tars StorageService)

#target_link_libraries(mysqlclient ssl crypto nghttp2-static)
