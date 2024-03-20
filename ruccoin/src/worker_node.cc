//
// Created by flt on 3/18/24.
//

#include "worker_node.h"
#include <nlohmann/json.hpp>
#include "utils.h"
#include <unistd.h>

using json = nlohmann::json;

ruccoin::WorkerNode::WorkerNode():inited_(false) {};


void ruccoin::WorkerNode::Init(uint32_t port) {
    my_port_ = port;
    coin_node_port_ = port+1;
}

std::string ruccoin::WorkerNode::DoMining(const BlockHeader &bh) {
    uint32_t secs = GenRandom(3, 20);
    SleepSeconds(secs);
    return GenRandom256();
}