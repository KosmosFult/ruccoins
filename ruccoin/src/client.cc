//
// Created by flt on 3/18/24.
//

#include "client.h"
#include "utils.h"
#include "config.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void ruccoin::client::ConnectNode(const std::string &addr, uint32_t port) {
    auto new_node = new rpc::client(addr, port);
    coin_nodes_.push_back(new_node);
}

void ruccoin::client::TestRun() {
    ConnectNode("127.0.0.1", 8080);

    auto current_time = std::chrono::system_clock::now();
    auto time_stamp = std::chrono::duration_cast<std::chrono::milliseconds>(current_time.time_since_epoch()).count();

    TX test_tx = {
            static_cast<uint64_t>(time_stamp), "addr1", "addr2", 123, "no"
    };
    Signate(test_tx);
    for (auto node: coin_nodes_) {
        node->call("AddTransx", test_tx);
    }
}

bool ruccoin::client::Signate(TX &transx) {
    assert(transx.signature.empty());
    std::string priv_key = GetPrivateKey(transx.from);
    assert(!priv_key.empty());

    std::string signature = CalSignature(transx, priv_key);
    transx.signature = signature;
    return true;
}

ruccoin::client::client() {

    // 解析配置文件
    std::fstream conf(ruccoin::config_path);
    if (!conf.is_open()) {
        std::cerr << "Can not open: \"" << config_path << "\"" << std::endl;
    }
    json conf_json = json::parse(conf);
    nodes_addr_ = conf_json["node_addr"];
    dbname_ = conf_json["client_db"];
    init_file_name_ = conf_json["user_init_file"];

    // 打开DB
    leveldb::Options options;
    options.create_if_missing = true;
    auto status = leveldb::DB::Open(options, dbname_, &addr2priv_);
    assert(status.ok());

}

ruccoin::client::~client() {
    for (auto &i: coin_nodes_)
        delete i;
    delete addr2priv_;
}

void ruccoin::client::SendTransx(const TX &transx) {
    assert(!transx.signature.empty());
    ConnectAllNodes();
    for (auto &node: coin_nodes_) {
        node->call("AddTransx", transx);
    }
    CloseAllNodes();
}

std::string ruccoin::client::GetPrivateKey(const std::string &addr) {
    std::string priv_key;
    auto status = addr2priv_->Get(leveldb::ReadOptions(), addr, &priv_key);
    if(!status.ok())
        return "";
    return priv_key;
}

void ruccoin::client::ConnectAllNodes() {
    // 连接所有coin node
    for (auto &addr: nodes_addr_) {
        auto flag = addr.find(':');

        std::string node_ip = addr.substr(0, flag);
        int node_port = atoi(addr.substr(flag + 1, addr.length() - flag).data());

        ConnectNode(node_ip, node_port);
    }

}

void ruccoin::client::GenUser(int n) {
    std::ofstream key_file(init_file_name_);
    if (!key_file.is_open()) {
        std::cerr << "Failed to open file for writing!" << std::endl;
        return;
    }

    std::fstream conf(ruccoin::config_path);
    if (!conf.is_open()) {
        std::cerr << "Can not open: \"" << config_path << "\"" << std::endl;
    }


    std::string priv_key, user_addr;
    // 将两个固定的特殊节点地址添加进去
    json conf_json = json::parse(conf);
    for(auto& addr : nodes_addr_){
        auto addr_pair = ParseAddr(addr);
        auto port_str = std::to_string(addr_pair.second);
        std::string priv_key = conf_json[port_str]["priv_key"];
        std::string user_addr = conf_json[port_str]["addr"];

        addr2priv_->Put(leveldb::WriteOptions(), user_addr , priv_key);
        std::string line = priv_key + "," + user_addr + "," + std::to_string(300);
        key_file << line << std::endl;
    }

    // 将奖励源添加
    priv_key = conf_json["reward_user"]["priv_key"];
    user_addr = conf_json["reward_user"]["addr"];
    addr2priv_->Put(leveldb::WriteOptions(), user_addr , priv_key);
    std::string line = priv_key + "," + user_addr + "," + "888888888";
    key_file << line << std::endl;

    for (int i = 0; i < n; i++) {
        auto key_pair = GenAddr();
        addr2priv_->Put(leveldb::WriteOptions(), key_pair.second , key_pair.first);
        std::string line = key_pair.first + "," + key_pair.second + "," + std::to_string(300);
        key_file << line << std::endl;
    }

    conf.close();
    key_file.close();
}

void ruccoin::client::Run() {

    std::string input;
    while (true) {
        std::cout << "~:";
        std::getline(std::cin, input);
        if (input == "quit") {
            break;
        }

        if (input == "gen"){
            GenUser(20);
            continue;
        }

        std::string from, to;
        double value;
        std::istringstream iss(input);
        if (!(iss >> from >> to >> value)) {
            std::cerr << "Invalid input format!" << std::endl;
            continue;
        }

        std::string priv_key = GetPrivateKey(from);
        if(priv_key.empty()){
            std::cerr << "No user address: " << from << std::endl;
            continue;
        }
        TX transx = {
            GetTimestamp(),
            from,
            to,
            value,
            ""
        };
        Signate(transx);
        SendTransx(transx);
        std::cout << "Transx send!" << std::endl;
    }

}

void ruccoin::client::CloseAllNodes() {
    for(auto& node : coin_nodes_)
        delete node;
    coin_nodes_.clear();
}

std::pair<std::string, uint32_t> ruccoin::client::ParseAddr(const std::string &addr) {
    auto flag = addr.find(':');
    std::string node_ip = addr.substr(0, flag);
    int node_port = atoi(addr.substr(flag + 1, addr.length() - flag).data());
    return std::make_pair(node_ip, node_port);
}
