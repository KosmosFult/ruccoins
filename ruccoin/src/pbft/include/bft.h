//
// Created by flt on 5/16/24.
//

#ifndef SIMPLEBITCOIN_BFT_H
#define SIMPLEBITCOIN_BFT_H

namespace PBFT {
    class Request {
    public:
        uint32_t client_id;
        uint64_t time_stamp;
        std::string body;

        Request() {}

        Request(uint64_t client_id, uint64_t time_stamp, std::string body) : client_id(client_id),
                                                                             time_stamp(time_stamp),
                                                                             body(std::move(body)) {
        }

        MSGPACK_DEFINE_ARRAY(client_id, time_stamp, body);
    };
}
#endif //SIMPLEBITCOIN_BFT_H
