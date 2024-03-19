//
// Created by flt on 3/18/24.
//

#include "coin_rpc.h"
#include "coin_node.h"

void BindAll(rpc::server& server){
    server.bind("AddTransx", &AddTransx);
    server.bind("MiningEnd", &MiningEnd);
}

bool AddTransx(TX& transx){
    auto& coin_node = ruccoin::CoinNode::getInstance();
    return coin_node.AddTransx(transx);
}

bool MiningEnd(){

    return true;
}

