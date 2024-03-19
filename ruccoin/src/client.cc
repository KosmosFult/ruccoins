//
// Created by flt on 3/18/24.
//

#include "client.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
const std::string config_path = "/home/flt/workspace/bitcoin/ruccoin/config.json";

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

ruccoin::client::client() {

    // 解析配置文件
    std::fstream conf(config_path);
    if(!conf.is_open()){
        std::cerr << "Can not open: \"" << config_path << "\"" << std::endl;
    }
    json conf_json = json::parse(conf);
    nodes_addr_ = conf_json["node_addr"];
    dbname_ = conf_json["client_db"];

    // 打开DB
    leveldb::Options options;
    options.create_if_missing = true;
    auto status = leveldb::DB::Open(options, dbname_, &addr2priv_);
    assert(status.ok());

}

ruccoin::client::~client() {
    for(auto &i : coin_nodes_)
        delete i;
}

void ruccoin::client::SendTransx(const TX &transx) {
    assert(!transx.signature.empty());

    for(auto& node : coin_nodes_){
        node->call("AddTransx", transx);
    }
}

std::string ruccoin::client::GetPrivateKey(const std::string &addr) {
    std::string priv_key;
    addr2priv_->Get(leveldb::ReadOptions(), addr, &priv_key);
    return priv_key;
}

void ruccoin::client::ConnectAllNodes() {
    // 连接所有coin node
    for(auto &addr : nodes_addr_){
        auto flag = addr.find(':');

        std::string node_ip = addr.substr(0, flag);
        int node_port = atoi(addr.substr(flag+1, addr.length()-flag).data());

        ConnectNode(node_ip, node_port);
    }

}
