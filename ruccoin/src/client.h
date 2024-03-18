//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_CLIENT_H
#define SIMPLEBITCOIN_CLIENT_H

#include <string>
#include <unordered_map>
#include "structure.h"
#include <rpc/client.h>

namespace ruccoin {

    class client{
    public:
        client();
        ~client();
        void Run();
        void TestRun();

    private:
        std::unordered_map<std::string, std::string> pri2pub_;  // 私钥到公钥映射
        std::vector<rpc::client*> coin_nodes_;   // 所有coin_node的rpc链接

        /**
         * @brief 根据交易公钥检索私钥，计算签名，并直接更新到该交易的签名
         * @param transx 待签名的交易
         * @return true 如果成果
         */
        bool Signate(TX& transx);

        /**
         * @brief 将交易发送给所有coin_node
         * @param transx 已签名的交易
         */
        void SendSignature(const TX& transx);

        /**
         * @brief 根据地址创建一个节点连接，并添加到coin_nodes
         * @param addr
         * @param port
         */
        void ConnectNode(const std::string& addr, uint32_t port);
    };
}


#endif //SIMPLEBITCOIN_CLIENT_H
