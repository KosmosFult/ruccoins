//
// Created by flt on 5/13/24.
//
#include "bft_node.h"
#include "bft_rpc.h"
#include "../cxxopts.h"
#include "bft_rpc.h"
int main(int argc, char *argv[]) {
    cxxopts::Options commandline_options("PBFT node", "For testing");
    uint32_t id;
    std::string config_json;
    commandline_options.add_options()
            ("i,id", "id of pbft node", cxxopts::value<uint32_t>(id))
            ("f,configfile", "configure file of PBFT", cxxopts::value<std::string>(config_json));
    commandline_options.parse(argc, argv);
    auto &pbft_node = PBFT::PBFTHandler::getInstance();
    pbft_node.Init(id, config_json);

    rpc::server server(pbft_node.GetPort());
    BindAll(server);
    server.async_run(8);
    std::cout << "PBFT node running on port[" << pbft_node.GetPort() << "]\n" << std::endl;
    while(true){}
    return 0;
}