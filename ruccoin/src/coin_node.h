//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_COIN_NODE_H
#define SIMPLEBITCOIN_COIN_NODE_H

#include <string>
#include "singleton.h"
#include "structure.h"
#include <leveldb/db.h>
#include <vector>
#include <rpc/client.h>

namespace ruccoin {
    class CoinNode : public PublicSingleton<CoinNode> {
        friend class PublicSingleton<CoinNode>;

    public:
        CoinNode(const CoinNode &) = delete;

        CoinNode &operator=(const CoinNode &) = delete;


        CoinNode();

        /**
         * @brief 初始化, 包括打开余额数据库, 建立到worker node的rpc连接等
         * @param dbname
         */
        void Init(const std::string &dbname);


        /**
         * @brief 先检查交易是否合法, 合法就添加到交易池
         * @param transx
         * @return true:合法且添加成功,false:不合法或添加错误
         */
        bool AddTransx(const TX &transx);

        /**
         * @brief 将交易池发送给worker node，开始挖矿，将异步调用的future对象保存到成员。
         * @return
         */
        bool Mining();

        /**
         * @brief 被挖矿程序调用以通知结果。函数中使用Mining时异步调用对象的get()获取nonce
         * @param nonce
         */
        void MiningEnd();


    private:
        std::string addr_;    // 节点自己的地址
        bool inited_;                // 是否已初始化
        std::string dbname_;  // 用户余额数据库目录
        leveldb::DB *balances_;     // 余额数据库
        std::vector<TX> tx_pool_;   // 交易池
        rpc::client *worker_;       // worker node的rpc连接
        std::vector<std::string> node_addr;  // 其余比特币节点的地址
        Block current_block_;
        std::future<clmdep_msgpack::object_handle> future;  // 用于存储异步调用的对象
        std::string block_chain_;  // 存储block chain的json文件

        /**
         * @brief 判断用户余额是否足够
         * @param from 用户的公钥地址
         * @param value 待判断金额（一般时转账金额）
         * @return true: 如果余额大于等于value; false 否则...
         */
        bool CheckBalance(const std::string &from, double value);

        /**
         * @brief 满足开始挖矿的条件，恒true，待完成
         * @return
         */
        bool MiningCond();

        /**
         * @brief 验证交易的签名是否合法
         * @param tx
         * @return true如果合法
         */
        bool CheckSignature(const TX &transx);

        /**
         * @brief 将当前交易池进行打包
         */
        void PackBlock();

        /**
         * @brief 向其它节点发送区块
         * @param block
         * @return
         */
        bool SendBlock(const Block &block);

    };
}


#endif //SIMPLEBITCOIN_COIN_NODE_H
