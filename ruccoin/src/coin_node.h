//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_COIN_NODE_H
#define SIMPLEBITCOIN_COIN_NODE_H

#include <string>
#include "singleton.h"
#include "tx.h"
#include <leveldb/db.h>
#include <vector>
#include <rpc/client.h>

namespace ruccoin {
    class CoinNode: public PublicSingleton<CoinNode> {
        friend class PublicSingleton<CoinNode>;
    public:
        CoinNode(const CoinNode&) = delete;
        CoinNode& operator=(const CoinNode&) = delete;

        /**
         * @brief 初始化, 包括打开余额数据库, 建立到worker node的rpc连接等
         * @param dbname
         */
        CoinNode(const std::string dbname);

        /**
         * @brief 先检查交易是否合法, 合法就添加到交易池
         * @param tranx
         * @return true:合法且添加成功,false:不合法或添加错误
         */
        bool AddTransx(const TX& tranx);

        /**
         * @brief 将交易池发送给worker node，开始挖矿
         * @return
         */
        bool Mining();
        
        bool SendBlock();

    private:
        const std::string dbname_;  // 用户余额数据库目录
        leveldb::DB* balances_;     // 余额数据库
        std::vector<TX> tx_pool_;   // 交易池
        rpc::client* worker_;       // worker node的rpc连接

        /**
         * @brief 判断用户余额是否足够
         * @param from 用户的公钥地址
         * @param value 待判断金额（一般时转账金额）
         * @return true: 如果余额大于等于value; false 否则...
         */
        bool CheckBalance(const std::string& from, double value);

        /**
         * @brief 验证交易的签名是否合法
         * @param tx
         * @return true如果合法
         */
        bool CheckSignature(const TX& transx);

    };
}


#endif //SIMPLEBITCOIN_COIN_NODE_H
