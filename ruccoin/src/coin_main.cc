//
// Created by flt on 3/18/24.
//

#include "coin_node.h"
#include "coin_rpc.h"

int main(int argc, char** argv) {
    int port = atoi(argv[1]);
    std::string dbname = "./coin_" + std::to_string(port) + "_db";
    auto& cnode = ruccoin::CoinNode::getInstance();
    cnode.Init(dbname);

    rpc::server coin_server(port);
    BindAll(coin_server);
    coin_server.run();
//    coin_server.async_run(2);
    return 0;
}