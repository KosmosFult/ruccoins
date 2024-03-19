//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_UTILS_H
#define SIMPLEBITCOIN_UTILS_H

#include "structure.h"
#include <string>
#include "picosha2.h"
#include <random>
#include <sstream>
#include <iomanip>

#include <random>

uint32_t GenRandom(int start, int end) {
    // 设置随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd()); // 使用 Mersenne Twister 引擎
    std::uniform_int_distribution<uint32_t> dist(start, end); // 定义一个整数均匀分布

    // 生成随机数并返回
    return dist(gen);
}

std::string GenRandom256() {
    // 使用随机设备生成器
    std::random_device rd;
    // 使用伪随机数生成器引擎
    std::default_random_engine rng(rd());
    // 使用分布器生成随机数，范围为 0 到 255
    std::uniform_int_distribution<int> dist(0, 255);

    // 生成 32 个字节的随机数据
    std::stringstream ss;
    for (int i = 0; i < 32; ++i) {
        // 生成随机字节
        int byte = dist(rng);
        // 将随机字节转换为十六进制字符串，并填充到 2 位，不足部分补零
        ss << std::setw(2) << std::setfill('0') << std::hex << byte;
    }

    // 返回生成的 64 位字符串表示
    return ss.str();
}


std::string RuccoinHash(std::string& data){
    std::string hash_hex_str;
    picosha2::hash256_hex_string(data, hash_hex_str);
    return hash_hex_str;
}

/**
 * @brief 生成私钥和地址
 * @return std::pair，第一个字符串是私钥，第二个是地址
 */
std::pair<std::string, std::string> GenAddr(){
    return std::make_pair("private key", "addr");
}

/**
 * @brief 验证交易的签名
 * @param transx
 * @return true如果签名验证通过
 */
bool ValidateSignature(const TX& transx);


/**
 * @brief 计算交易的签名
 * @param transx: 待计算的交易
 * @param private_key: 交易的"from"对应的私钥
 * @return 签名字符串
 */
std::string CalSignature(const TX& transx, const std::string& private_key);






std::string CalSignature(const TX& transx, const std::string& private_key){
    return "ToBeCompleted";
}
#endif //SIMPLEBITCOIN_UTILS_H
