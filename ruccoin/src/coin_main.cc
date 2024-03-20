//
// Created by flt on 3/18/24.
//

#include "coin_node.h"
#include "coin_rpc.h"
#include "utils.h"

int main(int argc, char** argv) {
    int port = atoi(argv[1]);
//    std::string dbname = "./coin_" + std::to_string(port) + "_db";
    auto& cnode = ruccoin::CoinNode::getInstance();
    cnode.Init(port);

    rpc::server coin_server(port);
    BindAll(coin_server);
    coin_server.async_run();

    while(true){
        SleepSeconds(2);
        if (cnode.MiningCond()) {
            cnode.PackBlock();
            cnode.Mining();
        }
    }
    return 0;
}