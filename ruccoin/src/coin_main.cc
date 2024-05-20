//
// Created by flt on 3/18/24.
//

#include "coin_node.h"
#include "coin_rpc.h"
#include "utils.h"
#include "cxxopts.h"



int main(int argc, char** argv) {
//    std::string dbname = "./coin_" + std::to_string(port) + "_db";
    auto& cnode = ruccoin::CoinNode::getInstance();
    cxxopts::Options commandline_options("coin node", "For testing");
    uint32_t id;
    std::string config_json;
    commandline_options.add_options()
            ("i,id", "id of pbft node", cxxopts::value<uint32_t>(id))
            ("f,configfile", "configure file of PBFT", cxxopts::value<std::string>(config_json));
    commandline_options.parse(argc, argv);

    cnode.Init(id, config_json);

    rpc::server coin_server(cnode.GetPort());
    BindAll(coin_server);
    coin_server.async_run(8);

    cnode.Run(argc, argv);

    return 0;
}