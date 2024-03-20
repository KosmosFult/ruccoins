//
// Created by flt on 3/19/24.
//

#ifndef SIMPLEBITCOIN_CONFIG_H
#define SIMPLEBITCOIN_CONFIG_H

#include <stdint.h>

namespace ruccoin{
    const uint32_t target = 2378178319;
    const std::string config_path = "/home/flt/workspace/bitcoin/ruccoin/config.json";
    const uint32_t reward_coin = 100;
    const std::string reward_src_priv_key = "0CC545CEB3A0E458BB77559BC354CBEA6C7D28193B6D436AE35EA358F4D09102";
    const std::string reward_src_addr = "0427b2899e02ddec870da2167e552e907b82016bf552896cac1ac4dc1b14b9c61068f1fe4dd8ada5527de20c1979535e1dacfba3be05782520ece8347d1919a067";
}
#endif //SIMPLEBITCOIN_CONFIG_H
