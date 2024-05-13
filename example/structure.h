//
// Created by flt on 5/13/24.
//

#ifndef SIMPLEBITCOIN_STRUCTURE_H
#define SIMPLEBITCOIN_STRUCTURE_H
#include <string>
#include <rpc/client.h>
#include "../ruccoin/src/structure.h"

enum class Mtype{prepare, commit};
MSGPACK_ADD_ENUM(Mtype);

class MessageHeader{
public:
    std::string key;
    int id;
    Mtype mtype;
    MSGPACK_DEFINE_ARRAY(key, id, mtype);
};
class Message{
    MessageHeader header;
    Block block;
    MSGPACK_DEFINE_ARRAY(header, block);
};

#endif //SIMPLEBITCOIN_STRUCTURE_H
