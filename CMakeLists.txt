cmake_minimum_required(VERSION 3.10)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# SET(CMAKE_C_COMPILER gcc)
# SET(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 17)


project(SimpleBitcoin)

add_subdirectory(thirdparty/rpclib)
add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/picosha2)
add_subdirectory(thirdparty/leveldb)
add_subdirectory(thirdparty/spdlog)
add_subdirectory(example)
add_subdirectory(ruccoin)