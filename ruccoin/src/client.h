//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_CLIENT_H
#define SIMPLEBITCOIN_CLIENT_H

#include <string>
#include <unordered_map>
#include "structure.h"
#include <rpc/client.h>
#include <leveldb/db.h>


namespace ruccoin {

    class client{
    public:
        client();
        ~client();

        /**
         * @brief 循环接收一行输入，分别为from, to, value;
         * 先获取当前时间戳，再将(timestamp, from, to ,value)使用from的私钥进行签名，最后得到签名完成的TX
         * 再用SendTransx将交易发出去
         */
        void Run();
        void TestRun();

    private:
        // std::unordered_map<std::string, std::string> pri2pub_;  // 私钥到公钥映射(使用leveldb, 因此已弃用)
        std::vector<std::string> nodes_addr_;    // 所有coin_node的地址
        std::vector<rpc::client*> coin_nodes_;   // 所有coin_node的rpc链接
        std::string dbname_;
        leveldb::DB* addr2priv_;                  // 地址到私钥的映射

        /**
         * @brief 获取地址对应的私钥，考虑调用leveldb
         * @param addr
         * @return
         */
        std::string GetPrivateKey(const std::string& addr);

        /**
         * @brief 根据交易公钥检索私钥，计算签名，并直接更新到该交易的签名
         * @param transx 待签名的交易
         * @return true 如果成果
         */
        bool Signate(TX& transx);

        /**
         * @brief 将交易发送给所有coin_node(必须已经过签名)
         * @param transx 已签名的交易
         */
        void SendTransx(const TX& transx);

        /**
         * @brief 根据地址创建一个节点连接，并添加到coin_nodes
         * @param addr
         * @param port
         */
        void ConnectNode(const std::string& addr, uint32_t port);

        void ConnectAllNodes();

        void CloseAllNodes();
    };
}


#endif //SIMPLEBITCOIN_CLIENT_H
