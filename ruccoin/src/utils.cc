//
// Created by flt on 3/20/24.
//

#include "utils.h"



void SleepSeconds(int seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(seconds));
}


uint64_t GetTimestamp(){
    return static_cast<uint64_t>(std::time(nullptr));
}

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






static std::string sha256(const std::string& data) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data.c_str(), data.size());
    SHA256_Final(hash, &sha256);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

// 获取交易的哈希值
std::string getHash(const TX& transx) {
    std::stringstream ss;
    // 此时签名应该是空
    ss << transx.time_stamp << transx.from << transx.to << transx.value;
    return sha256(ss.str());
}


std::string readPrivateKeyFromFile(const std::string& filePath) {
    std::ifstream file(filePath);
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}


// 私钥：一个随机数
std::string exportPrivateKeyHex(EC_KEY* ec_key) {
    // 从EC_KEY结构体中获取私钥部分。EC_KEY_get0_private_key函数返回
    // 一个指向私钥的BIGNUM结构体的指针。  任意精度的整数。这种数据类型可以存储非常大的整数值，避免了溢出问题
    const BIGNUM* priv_bn = EC_KEY_get0_private_key(ec_key);
    // 将私钥的BIGNUM结构体转换为十六进制字符串
    char* hex = BN_bn2hex(priv_bn);
    std::string priv_key_hex(hex);
    OPENSSL_free(hex);
    return priv_key_hex;
}

// 公钥椭圆曲线上的一个点
std::string exportPublicKeyHex(EC_KEY* ec_key) {
    // 将EC公钥转换为字节序列
    int len = i2o_ECPublicKey(ec_key, NULL);
    if (len <= 0) {
        return ""; // 失败时返回空字符串
    }

    unsigned char* pubKeyBytes = (unsigned char*)malloc(len);
    if (!pubKeyBytes) {
        return ""; // 确保内存分配成功
    }
    // 将公钥写入缓冲区
    unsigned char* p = pubKeyBytes;
    if (i2o_ECPublicKey(ec_key, &p) != len) {
        free(pubKeyBytes);
        return ""; // 转换失败时清理资源并返回
    }

    // 将公钥字节序列转换为十六进制字符串
    std::stringstream ss;
    for (int i = 0; i < len; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)pubKeyBytes[i];
    }

    free(pubKeyBytes); // 清理分配的内存

    return ss.str(); // 返回公钥的十六进制字符串
}


std::pair<std::string, std::string> GenAddr() {
    // 创建一个新的EC_KEY对象，指定使用NID_secp256k1椭圆曲线。
    // 这个曲线是加密货币，特别是比特币中常用的。
    EC_KEY* ec_key = EC_KEY_new_by_curve_name(NID_secp256k1);

    // 生成密钥对。这个函数将随机生成私钥，并计算相应的公钥。
    EC_KEY_generate_key(ec_key);

    // 转换为十六进制字符串。
    std::string private_key_hex = exportPrivateKeyHex(ec_key);
    std::string public_key = exportPublicKeyHex(ec_key);

    // 释放EC_KEY对象所占用的资源。
    EC_KEY_free(ec_key);

    return std::make_pair(private_key_hex, public_key);
}


// 从hex字符串转化成公钥
EC_KEY* hexToPublicKey(const std::string& hex) {
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    EC_POINT* pub_key = EC_POINT_hex2point(EC_KEY_get0_group(key), hex.c_str(), NULL, NULL);
    EC_KEY_set_public_key(key, pub_key);

    EC_POINT_free(pub_key);
    return key;
}

// 从hex字符串转化成私钥
EC_KEY* hexToPrivateKey(const std::string& hex) {
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    BIGNUM* priv_bn = BN_new();
    BN_hex2bn(&priv_bn, hex.c_str());
    EC_KEY_set_private_key(key, priv_bn);

    EC_POINT* pub_key = EC_POINT_new(EC_KEY_get0_group(key));
    EC_POINT_mul(EC_KEY_get0_group(key), pub_key, priv_bn, NULL, NULL, NULL);
    EC_KEY_set_public_key(key, pub_key);

    BN_free(priv_bn);
    EC_POINT_free(pub_key);
    return key;
}




bool ValidateSignature(const TX& transx) {
    std::string pubHex = transx.from;  // from存储了 私钥对应的公钥
    EC_KEY* publicKey = hexToPublicKey(pubHex); // Hex转换公钥
    if (!publicKey) {
        std::cerr << "Error creating public key from hex." << std::endl;
        return false;
    }

    std::string txHash = getHash(transx);
    std::vector<unsigned char> signature;
    size_t sig_len = transx.signature.length() / 2;
    signature.reserve(sig_len);
    //解析出签名数据。这个字节序列是签名的实际二进制表示。
    for (size_t i = 0; i < sig_len; i++) {
        unsigned int byte;
        sscanf(transx.signature.c_str() + 2 * i, "%02x", &byte);
        signature.push_back(static_cast<unsigned char>(byte));
    }

    // 验证签名
    int verify_status = ECDSA_verify(0, (unsigned char*)txHash.c_str(), txHash.length(), signature.data(), signature.size(), publicKey);
    if (verify_status != 1) {
        std::cerr << "Signature verification failed." << std::endl;
        EC_KEY_free(publicKey);
        return false;
    }

    EC_KEY_free(publicKey);
    return true;
}



std::string CalSignature(const TX& transx, const std::string& privHex) {
    EC_KEY* privateKey = hexToPrivateKey(privHex); // 使用之前定义的从Hex转换私钥的函数
    if (!privateKey) {
        std::cerr << "Error creating private key from hex." << std::endl;
        return "";
    }
    
    // 使用SHA-256 将这个交易映射成一个字符串
    std::string txHash = getHash(transx);
    unsigned char* signature = (unsigned char*)malloc(ECDSA_size(privateKey));
    unsigned int sig_len = 0;

    if (!ECDSA_sign(0, reinterpret_cast<const unsigned char*>(txHash.c_str()), txHash.length(), signature, &sig_len, privateKey)) {
        std::cerr << "Failed to sign transaction. Error code: " << ERR_get_error() << std::endl;
        EC_KEY_free(privateKey);
        free(signature);
        return "";
    }

    std::string signatureHex;
    for (unsigned int i = 0; i < sig_len; i++) {
        char hex[3];
        sprintf(hex, "%02x", signature[i]);
        signatureHex.append(hex);
    }

    EC_KEY_free(privateKey);
    free(signature);
    return signatureHex;
}


// not used 
EC_KEY* createKeyFromPrivateHex(const std::string& privHex) {
    EC_KEY* key = EC_KEY_new_by_curve_name(NID_secp256k1);
    BIGNUM* priv_bn = BN_new();
    BN_hex2bn(&priv_bn, privHex.c_str());
    EC_KEY_set_private_key(key, priv_bn);
    EC_POINT* pub_key = EC_POINT_new(EC_KEY_get0_group(key));
    EC_POINT_mul(EC_KEY_get0_group(key), pub_key, priv_bn, NULL, NULL, NULL);
    EC_KEY_set_public_key(key, pub_key);

    BN_free(priv_bn);
    EC_POINT_free(pub_key);

    return key;
}
