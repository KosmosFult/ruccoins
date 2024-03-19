//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_UTILS_H
#define SIMPLEBITCOIN_UTILS_H

#include "structure.h"
#include <string>
#include "picosha2.h"

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
