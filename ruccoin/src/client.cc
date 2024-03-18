//
// Created by flt on 3/18/24.
//

#include "client.h"
#include "utils.h"
#include <iostream>
#include <chrono>

void ruccoin::client::ConnectNode(const std::string& addr, uint32_t port){
    auto new_node = new rpc::client(addr, port);
    coin_nodes_.push_back(new_node);
}

void ruccoin::client::TestRun(){
    ConnectNode("127.0.0.1", 8080);

    auto current_time = std::chrono::system_clock::now();
    auto time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch()).count();

    TX test_tx = {
            static_cast<uint64_t>(time_stamp), "addr1", "addr2", 123, "no"
    };
    Signate(test_tx);
    for (auto node : coin_nodes_){
        node->call("AddTransx", test_tx);
    }
}

bool ruccoin::client::Signate(TX &transx) {
    assert(transx.signature.empty());
    auto find_itr = pri2pub_.find(transx.from);
    if(find_itr == pri2pub_.end()){
        std::cout << "Unknown \"From\": " << transx.from << std::endl;
        return false;
    }

    std::string signature = CalSignature(transx, find_itr->second);
    transx.signature = signature;
    return true;
}