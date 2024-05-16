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
#include <mutex>



namespace ruccoin {
    class CoinNode : public PublicSingleton<CoinNode> {
        friend class PublicSingleton<CoinNode>;

    public:
        CoinNode(const CoinNode &) = delete;

        CoinNode &operator=(const CoinNode &) = delete;

        ~CoinNode();
        CoinNode();

        /**
         * @brief 初始化, 包括打开余额数据库, 建立到worker node的rpc连接等
         * @param
         */
        void Init(uint32_t id, const std::string& config_json_path);


        /**
         * @brief 先检查交易签名, 合法就添加到交易池
         * @param transx
         * @return true:合法且添加成功,false:不合法或添加错误
         */
        bool AddTransx(const TX &transx);

        /**
         * @brief 将当前交易池进行打包, 注意会预执行一遍交易，如果存在执行过程中余额不足，则删除该交易
         * 1. 顺序预执行交易，若出现余额不足，则丢弃该交易
         * 2. 当执行的交易到达指定数量时，打包区块
         */
        void PackBlock();

        /**
         * @brief 将交易池发送给worker node，开始挖矿，将异步调用的future对象保存到成员。
         * @return
         */
        bool Mining();

        /**
         * @brief 满足开始发布区块的条件，恒true，待完成
         * @return
         */
        bool TryPublishCond();

        /**
         * @brief 先将区块持久化，向其它节点发送区块
         * @param block
         * @return
         */
        bool SendBlock();

        /**
         * @brief 接收其它节点发来的区块，验证合法性，合法就加入区块链，MINNING_MODE使用
         * @param block
         * @return
         */
        bool ReceiveBlock(Block& block);


        /**
         * @brief 用于使用PBFT共识机制。将p反序列化成Block，顺序执行交易，并将相同的交易从tx_pool中删除
         * @param p 提案
         * @return
         */
        void CommitProposal(const std::string& p);



        /**
         * @brief 被挖矿程序调用以通知结果。函数中使用Mining时异步调用对象的get()获取nonce
         * @param nonce
         */
        void MiningEnd();


        uint32_t GetPort() const{return port_;};



        void Run(int argc, char** argv);


    private:
        std::string local_addr_;    // 节点自己的地址
        std::string user_addr_;    // 账户地址
        std::string priv_key_;     // 账户私钥
        uint32_t id_;
        uint32_t port_;
        uint32_t worker_port_;
        uint32_t bft_node_port_;
        uint32_t bft_primary_id_;
        bool inited_;                // 是否已初始化
        std::string dbname_;  // 用户余额数据库目录
        std::string blockchain_dir_; // 存区块链的目录
        leveldb::DB *balances_;     // 余额数据库
        TXP tx_pool_;   // 交易池
        std::mutex pool_mutex_;
        rpc::client *worker_;       // worker node的rpc连接
        std::vector<std::string> node_addr;  // 其余比特币节点的地址
//        Block current_block_;      // 区块链上最新区块
        Block on_packing_block_;   // 正在打包的区块
        std::future<clmdep_msgpack::object_handle> future;  // 用于存储异步调用的对象

        // 以下方法用于将所有区块存到一个json，未来弃用
        std::string block_chain_json_;  // 存储block chain的json文件
        std::vector<Block> block_chain_;

        // 一个区块一个json
        std::vector<std::string> block_chain_hash_;  // 整个block chain的哈希

        void UpdateBlance(const TXL& transx_list);

        static std::string HeaderHash(const BlockHeader& bh);

        /**
         * @brief 判断用户余额是否足够
         * @param from 用户的公钥地址
         * @param value 待判断金额（一般时转账金额）
         * @return true: 如果余额大于等于value; false 否则...
         */
        bool CheckBalance(const std::string &from, double value);


        /**
         * @brief 验证交易的签名是否合法
         * @param tx
         * @return true如果合法
         */
        static bool CheckSignature(const TX &transx);

        /**
         * @brief 从交易池中移出指定交易
         * @param tx_list
         */
        void RemoveTxFromPool(const TXL& tx_list);

        bool CheckBlock(Block& block);

        void BlockProposal(const Block& block);




//        bool AddBlock(Block block);


        /**
         * @brief 获取当前merkle_root
         * @return
         */
        std::string GetMerkle();

        static std::pair<std::string, uint32_t> ParseAddr(const std::string& addr);

        void ReadUserData(const std::string& file_name);

        void ReadBlockChain();
        void ReadBlockChainHash(const std::string& genesis_block_hash);
        bool ReadBlock(const std::string& block_hash, Block& block);

        void WriteBlockChain();

        void WriteBlock(const Block& block);

    };
}


#endif //SIMPLEBITCOIN_COIN_NODE_H
