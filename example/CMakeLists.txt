cmake_minimum_required(VERSION 3.10)

project(example)

set(CMAKE_CXX_FLAGS -pthread)
include_directories(../thirdparty/rpclib/include)
include_directories(../thirdparty/json/include)
include_directories(../thirdparty/leveldb/include)


add_executable(client client.cc)
add_executable(server server.cc)
target_link_libraries(client rpc nlohmann_json leveldb)
target_link_libraries(server rpc nlohmann_json)



