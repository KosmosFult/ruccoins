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

void Prepare(PBFT::Message m){
    auto& pnode = PBFT::PBFTHandler::getInstance();
    pnode.Prepare(m);
}

void Commit(PBFT::Message m){
    auto& pnode = PBFT::PBFTHandler::getInstance();
    pnode.Commit(m);
}

void Reply(PBFT::Message m){
    auto& pnode = PBFT::PBFTHandler::getInstance();
    pnode.Reply(m);
}


static void BindAll(rpc::server& server){
    server.bind("GetRequest", &GetRequest);
    server.bind("Prepare", &Prepare);
    server.bind("Commit", &Commit);
    server.bind("Reply", &Reply);
}

#endif