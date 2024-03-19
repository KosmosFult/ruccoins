//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_WORKER_NODE_H
#define SIMPLEBITCOIN_WORKER_NODE_H

#include "singleton.h"
#include <rpc/client.h>

namespace ruccoin {
    class WorkerNode : public PublicSingleton<WorkerNode> {
        friend class PublicSingleton<WorkerNode>;

    public:
        WorkerNode(const WorkerNode &) = delete;

        WorkerNode &operator=(const WorkerNode &) = delete;
        WorkerNode();
        void Init();
    private:
        bool inited_;
        std::string addr_;
        rpc::client* coin_node_;  // 自己的coin node

    };
}


#endif //SIMPLEBITCOIN_WORKER_NODE_H
