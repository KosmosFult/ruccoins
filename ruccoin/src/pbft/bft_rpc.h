//
// Created by flt on 5/14/24.
//

#ifndef SIMPLEBITCOIN_BFT_RPC_H
#define SIMPLEBITCOIN_BFT_RPC_H

#include <rpc/server.h>
#include "bft_node.h"

void GetRequest(PBFT::Request r){
    auto& pnode = PBFT::PBFTHandler::getInstance();
    pnode.GetRequest(r);
}

static void BindAll(rpc::server& server){
    server.bind("GetRequest", &GetRequest);
}

#endif