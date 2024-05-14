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
#include <openssl/ec.h>
#include <openssl/pem.h>
#include <vector>
#include <string>
#include <iostream>
#include <openssl/sha.h>
#include <sstream>
#include <openssl/evp.h>
#include <fstream>
#include <openssl/ecdsa.h>
#include <openssl/err.h>
#include <utility>
#include <ctime>
#include <thread>


void SleepSeconds(int seconds);


uint64_t GetTimestamp();

uint32_t GenRandom(int start, int end);

std::string GenRandom256();

std::string RuccoinHash(const std::string& data);

/**
 * @brief 生成私钥和地址
 * @return std::pair，第一个字符串是私钥，第二个是地址
 */
std::pair<std::string, std::string> GenAddr();

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






static std::string sha256(const std::string& data);

// 获取交易的哈希值
std::string getHash(const TX& transx);


std::string readPrivateKeyFromFile(const std::string& filePath);



std::string exportPrivateKeyHex(EC_KEY* ec_key);



std::string exportPublicKeyHex(EC_KEY* ec_key);


std::pair<std::string, std::string> GenAddr();


// 从hex字符串转化成公钥
EC_KEY* hexToPublicKey(const std::string& hex);

// 从hex字符串转化成私钥
EC_KEY* hexToPrivateKey(const std::string& hex);


EC_KEY* createKeyFromPrivateHex(const std::string& privHex);
#endif //SIMPLEBITCOIN_UTILS_H
