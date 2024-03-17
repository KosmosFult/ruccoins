cmake_minimum_required(VERSION 3.10)

project(ruccoin)

set(CMAKE_CXX_FLAGS -pthread)
include_directories(../thirdparty/rpclib/include)
include_directories(../thirdparty/json/include)