//
// Created by flt on 3/18/24.
//

#ifndef SIMPLEBITCOIN_TX_H
#define SIMPLEBITCOIN_TX_H

#include <stdint.h>
#include <string>
// 交易
typedef struct TX{
    uint64_t time_stamp;
    std::string from;
    std::string to;
    double value;
    std::string signature;
} TX;

#endif //SIMPLEBITCOIN_TX_H
