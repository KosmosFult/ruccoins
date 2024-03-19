cmake_minimum_required(VERSION 3.10)

project(ruccoin)

set(CMAKE_CXX_FLAGS -pthread)
include_directories(../thirdparty/rpclib/include)
include_directories(../thirdparty/json/include)
include_directories(../thirdparty/picosha2)

add_executable(client ./src/client.cc ./src/client_main.cc)
target_link_libraries(client rpc nlohmann_json leveldb)

add_executable(coin_node ./src/coin_node.cc ./src/coin_main.cc ./src/coin_rpc.cc)
target_link_libraries(coin_node rpc nlohmann_json leveldb)

add_executable(worker_node ./src/worker_node.cc ./src/worker_main.cc ./src/worker_rpc.cc)
target_link_libraries(worker_node rpc)

add_executable(test_prog ./src/test_prog.cc)
