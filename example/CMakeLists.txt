cmake_minimum_required(VERSION 3.10)

project(example)

set(CMAKE_CXX_FLAGS -pthread)
include_directories(../thirdparty/rpclib/include)
include_directories(../thirdparty/json/include)
include_directories(../thirdparty/leveldb/include)


add_executable(client_example client.cc)
add_executable(server_example server.cc)
target_link_libraries(client_example rpc nlohmann_json leveldb)
target_link_libraries(server_example rpc nlohmann_json)



