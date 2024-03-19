//
// Created by flt on 3/18/24.
//

#include "coin_node.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

using json = nlohmann::json;
const std::string config_path = "/home/flt/workspace/bitcoin/ruccoin/config.json";

ruccoin::CoinNode::CoinNode(): inited_(false) {

}

void ruccoin::CoinNode::Init(const std::string& dbname){
    dbname_ = dbname;
    // Open config
    std::fstream f(config_path);
    if(!f.is_open()){
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
    if(!CheckSignature(transx))
        return false;
    tx_pool_.push_back(transx);
    return true;
}