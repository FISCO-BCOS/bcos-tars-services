cmake_minimum_required(VERSION 3.15)

project(bcos-LedgerService)

gen_server(bcos_tars LedgerService)

#target_link_libraries(mysqlclient ssl crypto nghttp2-static)
