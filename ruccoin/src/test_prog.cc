//
// Created by flt on 3/19/24.
//

#include "utils.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include <fstream>
using json = nlohmann::json;

void to_json(json& j, const BlockHeader& header) {
    j = json{
            {"height", header.height},
            {"target", header.target},
            {"prev_hash", header.prev_hash},
            {"hash", header.hash},
            {"merkle_root", header.merkle_root},
            {"nonce", header.nonce}
    };
}

void to_json(json& j, const TX& tx) {
    j = json{
            {"time_stamp", tx.time_stamp},
            {"from", tx.from},
            {"to", tx.to},
            {"value", tx.value},
            {"signature", tx.signature}
    };
}


// 将 Block 转换为 JSON 对象
void to_json(json& j, const Block& block) {
    j = json{
            {"header", block.header},
            {"transx_list", block.transx_list}
    };
}

void appendBlockToJsonFile(const std::string& filename, const Block& new_block) {
    // Open file in append mode
    std::ofstream outfile(filename, std::ios_base::app);
    if (!outfile.is_open()) {
        std::cerr << "Failed to open file for appending: " << filename << std::endl;
        return;
    }

    // Convert new_block to JSON format
    json new_block_json;
    new_block_json["header"]["height"] = new_block.header.height;
    new_block_json["header"]["target"] = new_block.header.target;
    new_block_json["header"]["prev_hash"] = new_block.header.prev_hash;
    new_block_json["header"]["hash"] = new_block.header.hash;
    new_block_json["header"]["merkle_root"] = new_block.header.merkle_root;
    new_block_json["header"]["nonce"] = new_block.header.nonce;

    for (const auto& tx : new_block.transx_list) {
        json tx_json;
        tx_json["time_stamp"] = tx.time_stamp;
        tx_json["from"] = tx.from;
        tx_json["to"] = tx.to;
        tx_json["value"] = tx.value;
        tx_json["signature"] = tx.signature;
        new_block_json["transx_list"].push_back(tx_json);
    }

    // Write new_block to file
    outfile << std::setw(2) << new_block_json << std::endl; // Pretty print JSON with indentation
    std::cout << "New block appended to file successfully." << std::endl;
}

int main() {
    std::string ss = "djakjfkajfljaflajf";
    std::cout << RuccoinHash(ss).length() << std::endl;
    std::cout << RuccoinHash(ss) << std::endl;
    std::pair<std::string, std::string> keys = GenAddr();
    std::string private_key = keys.first;
    std::string public_key = keys.second;
    std::cout << "Private Key: " << private_key << std::endl;
    std::cout << "Public Ke: " << public_key << std::endl;


    // 创建交易
    TX transx;
    transx.time_stamp = 1234567890; // 示例时间戳
    transx.from = public_key; // 这里应该是从公钥生成的地址
    transx.to = "jkajkdjakjdkajdkajdk";
    transx.value = 100.0; // 示例交易金额

    // 计算签名
    transx.signature = CalSignature(transx, private_key);
    std::cout << "Generated Signature: " << transx.signature << std::endl;
    // 验证签名
    bool isValid = ValidateSignature(transx);
    if (isValid) {
        std::cout << "Signature is valid." << std::endl;
    } else {
        std::cout << "Signature is NOT valid." << std::endl;
    }

    transx.signature[0] = 'd';
    std::cout << "Generated Signature: " << transx.signature << std::endl;
    // 验证签名
    isValid = ValidateSignature(transx);
    if (isValid) {
        std::cout << "Signature is valid." << std::endl;
    } else {
        std::cout << "Signature is NOT valid." << std::endl;
    }

    std::cout << GetTimestamp() << std::endl;

    std::ifstream bc("/home/flt/workspace/bitcoin/ruccoin/BlockChain.json");
    json bc_josn = json::parse(bc);

    std::vector<Block> blockchain;

    for(auto& block: bc_josn){
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
        for (const auto& txData : block["transx_list"]) {
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
        blockchain.push_back(cb);
    }

    std::ofstream new_json("/home/flt/workspace/bitcoin/ruccoin/BlockChain2.json");
    json jl = json::array();
    for(auto& block: blockchain){
        json j = block;
        jl.push_back(j);
    }
    new_json << std::setw(2) << jl << std::endl;
    new_json.close();
    return 0;

}