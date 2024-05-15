cmake_minimum_required(VERSION 3.10)

project(ruccoin)

set(CMAKE_CXX_FLAGS -pthread)
set(CMAKE_CXX_FLAGS -Wno-deprecated-declarations)
include_directories(../thirdparty/rpclib/include)
include_directories(../thirdparty/json/include)
include_directories(../thirdparty/picosha2)

option(LOGTX_SWITCH "print tx when receive" OFF)
option(MINING_MODE_SWITCH "use mining to reach consensus" OFF)

if(LOGTX_SWITCH)
    add_definitions(LOGTX)
endif (LOGTX_SWITCH)

if(MINING_MODE_SWITCH)
    add_definitions(MINING_MODE)
endif (MINING_MODE_SWITCH)

if(NOT TARGET spdlog)
    # Stand-alone build
    find_package(spdlog REQUIRED)
endif()

# 查找OpenSSL库
find_package(OpenSSL REQUIRED)

# 添加OpenSSL包含目录
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(client ./src/client.cc ./src/client_main.cc
        src/utils.cc)
target_link_libraries(client rpc nlohmann_json leveldb ${OPENSSL_LIBRARIES})

add_executable(coin_node ./src/coin_node.cc ./src/coin_main.cc ./src/coin_rpc.cc
        src/utils.cc ./src/pbft/bft_node.cc)
target_link_libraries(coin_node rpc nlohmann_json leveldb ${OPENSSL_LIBRARIES})

add_executable(worker_node ./src/worker_node.cc ./src/worker_main.cc ./src/worker_rpc.cc
        src/utils.cc)
target_link_libraries(worker_node rpc ${OPENSSL_LIBRARIES})

add_executable(test_prog ./src/test_prog.cc
        src/utils.cc)
target_link_libraries(test_prog ${OPENSSL_LIBRARIES} nlohmann_json)

add_executable(bft_main ./src/pbft/bft_node.cc ./src/pbft/bft_node.cc ./src/utils.cc ./src/pbft/bft_main.cc)
target_link_libraries(bft_main rpc nlohmann_json ${OPENSSL_LIBRARIES} spdlog::spdlog)

add_executable(test_pbft ./test/test_pbft.cc)
target_link_libraries(test_pbft rpc spdlog::spdlog)
