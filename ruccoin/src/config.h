//
// Created by flt on 3/19/24.
//

#ifndef SIMPLEBITCOIN_CONFIG_H
#define SIMPLEBITCOIN_CONFIG_H

#include <stdint.h>

namespace ruccoin{
    const uint32_t target = 2378178319;
    const std::string config_path = "/home/flt/workspace/bitcoin/ruccoin/config.json";
    const std::string reward_addr = std::string(130, '0');
    const uint32_t reward_coin = 100;
}
#endif //SIMPLEBITCOIN_CONFIG_H
