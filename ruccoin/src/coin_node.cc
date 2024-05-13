//
// Created by flt on 3/18/24.
//

#include "coin_node.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <list>
#include "config.h"
#include "utils.h"

using json = nlohmann::json;
namespace fs = std::filesystem;

void to_json(json &j, const BlockHeader &header) {
    j = json{
            {"height",      header.height},
            {"target",      header.target},
            {"prev_hash",   header.prev_hash},
            {"hash",        header.hash},
            {"merkle_root", header.merkle_root},
            {"nonce",       header.nonce}
    };
}

void to_json(json &j, const TX &tx) {
    j = json{
            {"time_stamp", tx.time_stamp},
            {"from",       tx.from},
            {"to",         tx.to},
            {"value",      tx.value},
            {"signature",  tx.signature}
    };
}


void to_json(json &j, const Block &block) {
    j = json{
            {"header",      block.header},
            {"transx_list", block.transx_list}
    };
}



static void StartWorkerNode(int port, const char* executable_dir) {
    std::cout << "Starting worker node..." << std::endl;
    char cmd[100];
    sprintf(cmd, "%s/worker_node %d %c", executable_dir, port, '&');
//    sprintf(cmd, "%s/worker_node %d", executable_dir, port);
    std::system(cmd);
}

ruccoin::CoinNode::CoinNode() : inited_(false) {

}

void ruccoin::CoinNode::Init(uint32_t port) {
    port_ = port;
    worker_port_ = port + 1;
    local_addr_ = "127.0.0.1:" + std::to_string(port);
    // Open config
    std::fstream f(ruccoin::config_path);
    if (!f.is_open()) {
        std::cerr << "Can not open file \"/home/flt/workspace/bitcoin/ruccoin/config.json\"" << std::endl;
        return;
    }

    // 将其余节点的地址加入到地址表
    json conf_json = json::parse(f);
    for (auto &addr: conf_json["node_addr"]) {
        if (addr != local_addr_)
            node_addr.push_back(addr);
    }


    // 初始化自己的私钥和地址
    user_addr_ = conf_json[std::to_string(port)]["addr"];
    priv_key_ = conf_json[std::to_string(port)]["priv_key"];
    std::cout << "[priv_key]: " << priv_key_.substr(0, 8) << "\n[addr]: " << user_addr_.substr(0, 8) << "\n"
              << std::endl;

    std::string env_path = conf_json["test_env_path"];

//    // 区块链文件
//    block_chain_json_ = test_env_path + "/coin_" + std::to_string(port_) + "_blockchain.json";
//    ReadBlockChain();

    blockchain_dir_ = env_path + "/coin_" + std::to_string(port_) + "/blockchain";
    ReadBlockChainHash(conf_json["genesis_block"]);

    // Open database
    dbname_ = env_path + "/coin_" + std::to_string(port_) + "/leveldb";
    leveldb::Options options;
    options.create_if_missing = true;
    auto status = leveldb::DB::Open(options, dbname_, &balances_);
    assert(status.ok());


    // 从初始化配置中读取用户数据
    ReadUserData(conf_json["user_init_file"]);

    f.close();
    inited_ = true;
}

bool ruccoin::CoinNode::AddTransx(const TX &transx) {

    // 不应该在这里检查而应该在执行的时候检查
//    if (!CheckBalance(transx.from, transx.value))
//        return false;

    if (!CheckSignature(transx))
        return false;
    tx_pool_.push_back(transx);

#ifdef  LOGTX
    std::cout << "Get transx\n" << "[From]: " + transx.from.substr(0, 8) + "\n"
              << "[To]:" + transx.to.substr(0, 8) + "\n" << std::endl;
#endif

    return true;
}


bool ruccoin::CoinNode::TryPublishCond() {
    return tx_pool_.size() >= 20;
}

bool ruccoin::CoinNode::Mining() {
    std::cout << "Mining start!" << std::endl;
    worker_ = new rpc::client("127.0.0.1", worker_port_);
    std::string nonce = worker_->call("DoMining", on_packing_block_.header).as<std::string>();
    on_packing_block_.header.nonce = nonce;
    std::string header_hash = HeaderHash(on_packing_block_.header);
    on_packing_block_.header.hash = header_hash;
    std::cout << "Mining complete!" << std::endl;

    tx_pool_.erase(tx_pool_.begin(), tx_pool_.begin() + (on_packing_block_.transx_list.size()) - 1);


    // 说明未接受新的区块
    if (block_chain_.back().header.hash == on_packing_block_.header.prev_hash) {
        std::cout << "New Block " << on_packing_block_.header.hash.substr(0, 8) << "\n" << std::endl;
        block_chain_.push_back(on_packing_block_);
        UpdateBlance(on_packing_block_.transx_list);
    } else{
        std::cout << "Block expired!\n" << std::endl;
        delete worker_;
        return false;
    }
    delete worker_;
    return true;
}


std::string ruccoin::CoinNode::GetMerkle() {
    return GenRandom256();
}

void ruccoin::CoinNode::PackBlock() {
    assert(TryPublishCond());
    on_packing_block_.header = {
            block_chain_.back().header.height + 1,
            ruccoin::target,
            block_chain_.back().header.hash,
            GenRandom256(),
            GetMerkle(),
            "nonce"
    };
    on_packing_block_.transx_list = tx_pool_;
    TX reward_tx = {
            GetTimestamp(),
            ruccoin::reward_src_addr,
            user_addr_,
            ruccoin::reward_coin,
            ""
    };
    auto sign = CalSignature(reward_tx, ruccoin::reward_src_priv_key);
    reward_tx.signature = sign;
    assert(ValidateSignature(reward_tx));
    on_packing_block_.transx_list.push_back(reward_tx);
}

bool ruccoin::CoinNode::CheckBalance(const std::string &from, double value) {
    std::string balance;
    auto status = balances_->Get(leveldb::ReadOptions(), from, &balance);
    if (status.IsNotFound()) {
        std::cerr << "User \"" << from.substr(0, 8) << "\" not found\n" << std::endl;
        return false;
    }
    if (std::stod(balance) < value)
        return false;
    return true;
}

bool ruccoin::CoinNode::CheckSignature(const TX &transx) {
    return ValidateSignature(transx);
}

std::string ruccoin::CoinNode::HeaderHash(const BlockHeader &bh) {
    std::string hdata =
            std::to_string(bh.height) + std::to_string(bh.target) +
            bh.prev_hash + bh.merkle_root + bh.nonce;
    return RuccoinHash(hdata);
}

std::pair<std::string, uint32_t> ruccoin::CoinNode::ParseAddr(const std::string &addr) {
    auto flag = addr.find(':');
    std::string node_ip = addr.substr(0, flag);
    int node_port = atoi(addr.substr(flag + 1, addr.length() - flag).data());
    return std::make_pair(node_ip, node_port);
}

void ruccoin::CoinNode::ReadUserData(const std::string &file_name) {
    assert(balances_ != nullptr);
    std::ifstream file(file_name);

    if (!file.is_open()) {
        std::cerr << "Failed to open the file " + file_name << std::endl;
        return;
    }

    std::string pub_key, balance;

    std::string line;
    while (std::getline(file, line)) {
        size_t pos1 = line.find(',');
        if (pos1 == std::string::npos) {
            std::cerr << "Invalid format: " << line << std::endl;
            continue;
        }
        size_t pos2 = line.find(',', pos1 + 1);
        if (pos2 == std::string::npos) {
            std::cerr << "Invalid format: " << line << std::endl;
            continue;
        }

        pub_key = line.substr(pos1 + 1, pos2 - pos1 - 1);
        balance = line.substr(pos2 + 1);
        auto status = balances_->Put(leveldb::WriteOptions(), pub_key, balance);
        assert(status.ok());
    }
}

ruccoin::CoinNode::~CoinNode() {
    delete worker_;
    delete balances_;
    WriteBlockChain();
}

void ruccoin::CoinNode::ReadBlockChain() {
    assert(!block_chain_json_.empty());
    std::ifstream bc(block_chain_json_);
    if (!bc.is_open()) {
        std::cerr << block_chain_json_ << " not exists\n" << std::endl;
        return;
    }

    json bc_josn = json::parse(bc);

    for (auto &block: bc_josn) {
        // Parse block header
        BlockHeader header;
        header.height = block["header"]["height"];
        header.target = block["header"]["target"];
        header.prev_hash = block["header"]["prev_hash"];
        header.hash = block["header"]["hash"];
        header.merkle_root = block["header"]["merkle_root"];
        header.nonce = block["header"]["nonce"];

        // Parse transactions
        TXL transx_list;
        for (const auto &txData: block["transx_list"]) {
            TX tx;
            tx.time_stamp = txData["time_stamp"];
            tx.from = txData["from"];
            tx.to = txData["to"];
            tx.value = txData["value"];
            tx.signature = txData["signature"];
            transx_list.push_back(tx);
        }

        // Create block and add it to blockchain
        Block cb;
        cb.header = header;
        cb.transx_list = transx_list;
        block_chain_.push_back(cb);
    }
    bc.close();

    std::cout << "Block chain loading\n" << std::endl;
}

void ruccoin::CoinNode::WriteBlockChain() {
    std::ofstream json_file(block_chain_json_);
    json jl = json::array();
    for (auto &block: block_chain_) {
        json jb = block;
        jl.push_back(jb);
    }
    json_file << std::setw(2) << jl << std::endl;
    json_file.close();
    std::cout << "Block chain writing\n" << std::endl;
}

void ruccoin::CoinNode::WriteBlock(const Block& block) {
    json jblock(block);
    std::ofstream json_file(block.header.hash + ".json");
    json_file << std::setw(2) << jblock << std::endl;
    json_file.close();
}

bool ruccoin::CoinNode::SendBlock() {
    WriteBlockChain();
    for (auto &addr: node_addr) {
        auto addr_pair = ParseAddr(addr);
        rpc::client connect(addr_pair.first, addr_pair.second);
        connect.call("AddBlock", on_packing_block_);
    }
    return true;
}

bool ruccoin::CoinNode::CheckBlock(Block &block) {
    if (block.header.prev_hash != block_chain_.back().header.hash) {
        return false;
    }

    for(auto& tx: block.transx_list){
        if(!CheckSignature(tx) || !CheckBalance(tx.from, tx.value))
            return false;
    }
    return true;
}

bool ruccoin::CoinNode::ReceiveBlock(Block &block) {
    if (!CheckBlock(block)) {
        std::cout << "Block " << block.header.hash.substr(0, 8) << " is not valid!\n" << std::endl;
        return false;
    } else {
        std::cout << "Block " << block.header.hash.substr(0, 8) << " added!\n" << std::endl;
        block_chain_.push_back(block);
        UpdateBlance(block.transx_list);
        WriteBlockChain();
    }
    return true;
}

void ruccoin::CoinNode::UpdateBlance(const TXL &transx_list) {
    std::string from_balance, to_balance;
    leveldb::Status status;
    for (auto &tx: transx_list) {
        status = balances_->Get(leveldb::ReadOptions(), tx.from, &from_balance);
        assert(status.ok());
        status = balances_->Get(leveldb::ReadOptions(), tx.to, &to_balance);
        assert(status.ok());
        balances_->Put(leveldb::WriteOptions(),
                       tx.from, std::to_string(std::stod(from_balance) - tx.value));
        balances_->Put(leveldb::WriteOptions(),
                       tx.to, std::to_string(std::stod(to_balance) + tx.value));
    }
}

void ruccoin::CoinNode::ReadBlockChainHash(const std::string& genesis_block_hash) {
    block_chain_hash_.clear();
    std::list<std::pair<std::string, std::string>> hash_prehash;

    for (const auto& entry : fs::directory_iterator(blockchain_dir_)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            if(entry.path().filename().stem() == genesis_block_hash)
                continue;
            std::ifstream block(entry.path());
            if (!block.is_open()) {
                std::cerr << entry.path() << " not exists\n" << std::endl;
                return;
            }

            json block_json = json::parse(block);
            hash_prehash.emplace_back(entry.path().filename().stem(), block_json["header"]["prev_hash"]);
        }
    }
    block_chain_hash_.push_back(genesis_block_hash);
    while(!hash_prehash.empty() ){
        auto it = hash_prehash.begin();
        for(; it != hash_prehash.end(); it++){
            if(it->second == block_chain_hash_.back()){
                block_chain_hash_.push_back(it->first);
                hash_prehash.erase(it);
                break;
            }
        }
        if(it == hash_prehash.end())  // 有冗余block(不能把所有文件建立成一个链)
            break;
    }
}

void ruccoin::CoinNode::Run(int argc, char** argv) {

    fs::path prog_path(argv[0]);
    fs::path prog_dir = prog_path.parent_path();
#ifdef  MINIG_MODE
    StartWorkerNode(port+1, prog_dir.c_str());
#endif
    bool send_flag = false;
    while(true){
        SleepSeconds(2);
        if (TryPublishCond()) {
            PackBlock();
            send_flag = Mining();
            if(send_flag)
                SendBlock();
        }
    }
}


