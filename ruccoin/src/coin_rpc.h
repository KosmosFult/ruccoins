//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_COIN_RPC_H
#define SIMPLEBITCOIN_COIN_RPC_H

#include <rpc/server.h>


/**
 * @brief 将所有函数绑定到rpc server
 * @param server
 */
void BindAll(rpc::server& server);




#endif //SIMPLEBITCOIN_COIN_RPC_H
