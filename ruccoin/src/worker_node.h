//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_WORKER_NODE_H
#define SIMPLEBITCOIN_WORKER_NODE_H

#include "singleton.h"
#include <rpc/client.h>
#include "structure.h"

namespace ruccoin {
    class WorkerNode : public PublicSingleton<WorkerNode> {
        friend class PublicSingleton<WorkerNode>;

    public:
        WorkerNode(const WorkerNode &) = delete;

        WorkerNode &operator=(const WorkerNode &) = delete;

        WorkerNode();
        void Init(uint32_t port);
        void Run();
        std::string DoMining(const BlockHeader& bh);
    private:
        bool inited_;
        uint32_t my_port_;
        uint32_t coin_node_port_;
        rpc::client* coin_node_;  // 自己的coin node

    };
}


#endif //SIMPLEBITCOIN_WORKER_NODE_H
