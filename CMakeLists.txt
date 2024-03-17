cmake_minimum_required(VERSION 3.10)
# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# SET(CMAKE_C_COMPILER gcc)
# SET(CMAKE_CXX_COMPILER g++)

set(CMAKE_CXX_STANDARD 14)


project(SimpleBitcoin)

add_subdirectory(thirdparty/rpclib)
add_subdirectory(thirdparty/json)
add_subdirectory(thirdparty/picosha2)
add_subdirectory(example)