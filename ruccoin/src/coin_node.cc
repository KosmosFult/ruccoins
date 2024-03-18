//
// Created by flt on 3/18/24.
//

#include "coin_node.h"

ruccoin::CoinNode::CoinNode(): inited_(false) {

}

void ruccoin::CoinNode::Init(const std::string& dbname){
    inited_ = true;
    dbname_ = dbname;
    leveldb::Options options;
    options.create_if_missing = true;
    auto status = leveldb::DB::Open(options, dbname_, &balances_);
    assert(status.ok());
}

bool ruccoin::CoinNode::AddTransx(const TX &transx) {
    if(!CheckSignature(transx))
        return false;
    tx_pool_.push_back(transx);
    return true;
}