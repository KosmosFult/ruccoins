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



/**
 * @brief 计算交易的签名
 * @param transx: 待计算的交易
 * @param private_key: 交易的"from"对应的私钥
 * @return 签名字符串
 */

#include <openssl/ec.h>
#include <openssl/pem.h>
#include <vector>
#include <string>
#include <iostream>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <openssl/evp.h>

class TxInput {
public:
    std::string txid; // 引用的UTXO所在的交易ID
    int outIndex; // 引用的UTXO在其交易中的索引位置
    std::vector<unsigned char> signature; // 签名
    std::string pubkey; // 公钥
    TxInput(const std::string& txid, int outIndex, const std::string& pubkey) : txid(txid), outIndex(outIndex), pubkey(pubkey) {}
};

class TxOutput {
public:
    int value; // 转账金额
    std::string pubkeyHash; // 接收方的公钥哈希
    TxOutput(int value, const std::string& pubkeyHash) : value(value), pubkeyHash(pubkeyHash) {}
};


class Transaction {
public:
    std::vector<TxInput> inputs;
    std::vector<TxOutput> outputs;
    unsigned int timestamp; // 交易创建的时间戳

    Transaction(const std::vector<TxInput>& inputs, const std::vector<TxOutput>& outputs, unsigned int timestamp) : inputs(inputs), outputs(outputs), timestamp(timestamp) {}

    // 获取交易的哈希值
    std::string getHash() const {
        std::stringstream ss;
        for (const auto& input : inputs) {
            ss << input.txid << input.outIndex << input.pubkey;
        }
        for (const auto& output : outputs) {
            ss << output.value << output.pubkeyHash;
        }
        ss << timestamp;
        std::string txData = ss.str();

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(txData.c_str()), txData.size(), hash);

        std::stringstream hashString;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            hashString << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }
        return hashString.str();
    }
};


std::vector<unsigned char> signMessage(const std::string& message, EC_KEY* privateKey) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_PKEY* signing_key = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(signing_key, privateKey);
    const EVP_MD* md = EVP_sha256();

    EVP_DigestSignInit(ctx, nullptr, md, nullptr, signing_key);
    EVP_DigestSignUpdate(ctx, reinterpret_cast<const unsigned char*>(message.c_str()), message.length());

    size_t sig_len;
    EVP_DigestSignFinal(ctx, nullptr, &sig_len);

    std::vector<unsigned char> signature(sig_len);
    EVP_DigestSignFinal(ctx, signature.data(), &sig_len);

    EVP_MD_CTX_free(ctx);
    // signing_key is linked to privateKey, which should not be freed here

    signature.resize(sig_len);
    return signature;
}




void signTransaction(Transaction& tx, EC_KEY* privateKey) {
    //getHash()用于获取交易的哈希值
    std::string txHash = tx.getHash();
    for (size_t i = 0; i < tx.inputs.size(); ++i) {
        std::vector<unsigned char> sig = signMessage(txHash, privateKey);
    }
}

// 使用公钥验证签名
bool verifySignature(const std::string& message, const std::vector<unsigned char>& signature, EC_KEY* publicKey) {
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    EVP_PKEY* verification_key = EVP_PKEY_new();
    EVP_PKEY_assign_EC_KEY(verification_key, EC_KEY_dup(publicKey)); // Duplicate key for safety
    const EVP_MD* md = EVP_sha256();

    EVP_DigestVerifyInit(ctx, nullptr, md, nullptr, verification_key);
    EVP_DigestVerifyUpdate(ctx, message.c_str(), message.length());

    bool result = EVP_DigestVerifyFinal(ctx, signature.data(), signature.size());

    EVP_MD_CTX_free(ctx);
    EVP_PKEY_free(verification_key); // Safe to free now

    return result;
}




// 用于本地测试
int main() {
    // 创建EC密钥对
    EC_KEY* eckey = EC_KEY_new_by_curve_name(NID_secp256k1);
    if (!EC_KEY_generate_key(eckey)) {
        std::cerr << "Error generating the EC key." << std::endl;
        return -1;
    }

    // 构建一个示例交易
    TxInput input("some_previous_txid", 0, "sender's public key");
    TxOutput output(100, "receiver's public key hash");
    std::vector<TxInput> inputs = { input };
    std::vector<TxOutput> outputs = { output };
    Transaction tx(inputs, outputs, 1234567890); // 使用示例时间戳

    // 签名交易
    // 注意: signTransaction函数需要更新以实际附加签名到交易
    std::string txHash = tx.getHash();
    std::vector<unsigned char> signature = signMessage(txHash, eckey);

    // 输出签名的十六进制形式
    std::cout << "Signature: ";
    for (unsigned char c : signature) {
        std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)c;
    }
    std::cout << std::endl;

    // 验证签名
    bool isValid = verifySignature(txHash, signature, eckey);
    if (isValid) {
        std::cout << "Signature verified successfully." << std::endl;
    }
    else {
        std::cout << "Signature verification failed." << std::endl;
    }
    // 清理
    EC_KEY_free(eckey);
    return 0;
}
#endif //SIMPLEBITCOIN_UTILS_H
