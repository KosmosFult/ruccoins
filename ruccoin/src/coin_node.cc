//
// Created by flt on 3/18/24.
//

#include "coin_node.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include "utils.h"
#include "config.h"

using json = nlohmann::json;
const std::string config_path = "/home/flt/workspace/bitcoin/ruccoin/config.json";

ruccoin::CoinNode::CoinNode() : inited_(false) {

}

void ruccoin::CoinNode::Init(const std::string &dbname) {
    dbname_ = dbname;
    // Open config
    std::fstream f(config_path);
    if (!f.is_open()) {
        std::cerr << "Can not open file \"/home/flt/workspace/bitcoin/ruccoin/config.json\"" << std::endl;
        return;
    }
    json conf_json = json::parse(f);
    node_addr = conf_json["node_addr"];

    // Open database
    leveldb::Options options;
    options.create_if_missing = true;
    auto status = leveldb::DB::Open(options, dbname_, &balances_);
    assert(status.ok());

    f.close();
    inited_ = true;
}

bool ruccoin::CoinNode::AddTransx(const TX &transx) {
    if (!CheckSignature(transx))
        return false;
    tx_pool_.push_back(transx);

    if(MiningCond()){
        PackBlock();

    }
    return true;
}


bool ruccoin::CoinNode::MiningCond() {
    return tx_pool_.size() >= 3;
}

bool ruccoin::CoinNode::Mining() {

}


std::string ruccoin::CoinNode::GetMerkle() {
    return GenRandom256();
}

void ruccoin::CoinNode::PackBlock() {
    assert(MiningCond());
    on_packing_block_.header = {
            current_block_.header.height + 1,
            ruccoin::target,
            current_block_.header.hash,
            GenRandom256(),
            GetMerkle(),
            "nonce"
    };
    on_packing_block_.transx_list = tx_pool_;
}

bool ruccoin::CoinNode::CheckBalance(const std::string &from, double value){
    return true;
}