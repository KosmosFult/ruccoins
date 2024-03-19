//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_TX_H
#define SIMPLEBITCOIN_TX_H

#include <stdint.h>
#include <string>
#include <rpc/msgpack.hpp>

struct TX{
    uint64_t time_stamp;
    std::string from;
    std::string to;
    double value;
    std::string signature;

    MSGPACK_DEFINE_ARRAY(time_stamp, from, to, value, signature);
};

using TXL = std::vector<TX>;

struct BlockHeader {
    uint64_t height;
    uint64_t target;      // 难度
    std::string prev_hash;
    std::string hash;
    std::string merkle_root;
    std::string nonce;

    MSGPACK_DEFINE_ARRAY(height, target, prev_hash, hash, merkle_root, nonce);
};

struct Block {
    BlockHeader header;
    TXL transx_list;

    MSGPACK_DEFINE_ARRAY(header, transx_list);
};


#endif //SIMPLEBITCOIN_TX_H
