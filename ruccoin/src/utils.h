//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_UTILS_H
#define SIMPLEBITCOIN_UTILS_H

#include "structure.h"
#include <string>
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


#endif //SIMPLEBITCOIN_UTILS_H
