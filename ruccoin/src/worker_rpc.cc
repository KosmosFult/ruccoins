//
// Created by flt on 3/18/24.
//

#include "worker_rpc.h"
#include "worker_node.h"

void BindAll(rpc::server& server){
    server.bind("DoMining", &DoMining);
}



std::string DoMining(BlockHeader bh){
    auto& worker = ruccoin::WorkerNode::getInstance();
    auto nonce = worker.DoMining(bh);

    return nonce;
}
