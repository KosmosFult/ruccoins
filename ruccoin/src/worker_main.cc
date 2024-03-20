//
// Created by flt on 3/18/24.
//

#include "worker_node.h"
#include <rpc/server.h>
#include "worker_rpc.h"
#include <iostream>

int main(int argc, char** argv){

    int port = atoi(argv[1]);
    auto& worker = ruccoin::WorkerNode::getInstance();
    worker.Init(port);
    rpc::server worker_server(port);
    BindAll(worker_server);
    std::cout << "[worker node]: On port " << port << std::endl;
    worker_server.run();
    return 0;
}