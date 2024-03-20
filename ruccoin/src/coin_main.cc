//
// Created by flt on 3/18/24.
//

#include "coin_node.h"
#include "coin_rpc.h"
#include "utils.h"

void StartWorkerNode(int port, const char* executable_dir) {
    std::cout << "Starting worker node..." << std::endl;
    char cmd[100];
    sprintf(cmd, "%s/worker_node %d %c", executable_dir, port, '&');
//    sprintf(cmd, "%s/worker_node %d", executable_dir, port);
    std::system(cmd);
}

int main(int argc, char** argv) {
    int port = atoi(argv[1]);
//    std::string dbname = "./coin_" + std::to_string(port) + "_db";
    auto& cnode = ruccoin::CoinNode::getInstance();
    cnode.Init(port);

    rpc::server coin_server(port);
    BindAll(coin_server);
    coin_server.async_run();

    // 获取当前程序的路径
    std::string executable_dir = argv[0];
    size_t lastSlash = executable_dir.find_last_of("/");
    if (lastSlash != std::string::npos) {
        executable_dir = executable_dir.substr(0, lastSlash);
    } else {
        executable_dir = ".";
    }

    if(argc > 2)
        StartWorkerNode(port+1, executable_dir.c_str());

    while(true){
        SleepSeconds(2);
        if (cnode.MiningCond()) {
            cnode.PackBlock();
            cnode.Mining();
            cnode.SendBlock();
        }
    }
    return 0;
}