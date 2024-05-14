//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_TX_H
#define SIMPLEBITCOIN_TX_H

#include <cstdint>
#include <string>
#include <rpc/msgpack.hpp>
#include <nlohmann/json.hpp>

struct TX{
    uint64_t time_stamp = 0;
    std::string from;
    std::string to;
    double value = 0.0;
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


static void to_json(nlohmann::json &j, const BlockHeader &header) {
    j = nlohmann::json{
            {"height",      header.height},
            {"target",      header.target},
            {"prev_hash",   header.prev_hash},
            {"hash",        header.hash},
            {"merkle_root", header.merkle_root},
            {"nonce",       header.nonce}
    };
}

static void from_json(const nlohmann::json &j, BlockHeader &header) {
    j.at("height").get_to(header.height);
    j.at("target").get_to(header.target);
    j.at("prev_hash").get_to(header.prev_hash);
    j.at("hash").get_to(header.hash);
    j.at("merkle_root").get_to(header.merkle_root);
    j.at("nonce").get_to(header.nonce);
}


static void to_json(nlohmann::json &j, const TX &tx) {
    j = nlohmann::json{
            {"time_stamp", tx.time_stamp},
            {"from",       tx.from},
            {"to",         tx.to},
            {"value",      tx.value},
            {"signature",  tx.signature}
    };
}

static void from_json(const nlohmann::json &j, TX &tx){
    j.at("time_stamp").get_to(tx.time_stamp);
    j.at("from").get_to(tx.from);
    j.at("to").get_to(tx.to);
    j.at("value").get_to(tx.value);
    j.at("signature").get_to(tx.signature);
}


static void to_json(nlohmann::json &j, const Block &block) {
    j = nlohmann::json{
            {"header",      block.header},
            {"transx_list", block.transx_list}
    };
}

static void from_json(const nlohmann::json &j, Block &block) {
    j.at("header").get_to(block.header);
    block.transx_list.clear();

    for (const auto &txData: j["transx_list"])
        block.transx_list.push_back(txData.template get<TX>());

}

#endif //SIMPLEBITCOIN_TX_H
