//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_COIN_RPC_H
#define SIMPLEBITCOIN_COIN_RPC_H

#include <rpc/server.h>
#include "structure.h"

/**
 * @brief 将所有函数绑定到rpc server
 * @param server
 */
void BindAll(rpc::server& server);


/**
 * @brief 由client调用, 将交易添加到交易池
 * @param transx
 * @return
 */
bool AddTransx(TX& transx);


/**
 * @brief 由coin node调用, 将区块添加到区块链
 * @param block
 * @return
 */
bool AddBlock(Block& block);


/**
 * @brief 由worker调用,通知挖矿完成, coin node因此将之前异步调用对象的结果取出
 * @return
 */
bool MiningEnd();


bool CheckProposal(std::string p);


#endif //SIMPLEBITCOIN_COIN_RPC_H
