//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_WORKER_RPC_H
#define SIMPLEBITCOIN_WORKER_RPC_H

#include <rpc/server.h>
#include "structure.h"

/**
 * @brief 将所有函数绑定到rpc server
 * @param server
 */
void BindAll(rpc::server& server);


/**
 * @brief 由worker node对应的coin node调用, 计算传入的block，返回结果nonce
 * @param block
 * @return 计算好的nonce
 */
std::string DoMining(Block block);

#endif //SIMPLEBITCOIN_WORKER_RPC_H
