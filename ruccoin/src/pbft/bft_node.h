//
// Created by flt on 5/12/24.
//

#ifndef SIMPLEBITCOIN_BFT_NODE_H
#define SIMPLEBITCOIN_BFT_NODE_H

#include <cstdint>
#include <rpc/client.h>
#include "../singleton.h"
#include "../structure.h"

enum class PBTF_MType {
    Preprepare,
    Prepare,
    Commit
};

enum class PBFT_PStat {
    pre_prepared, prepared, committed
};

MSGPACK_ADD_ENUM(PBTF_MType);
MSGPACK_ADD_ENUM(PBFT_PStat);

namespace PBFT {
    class Message;

    class Proposal;

    struct NodeMeta {
        uint32_t replica_id;
        std::string hostname;
        uint32_t port;
        std::string pub_key;
    };

    class Request {
    public:
        uint64_t client_id;
        uint64_t time_stamp;
        Block block;

        Request(uint64_t client_id,
                uint64_t time_stamp,
                const Block& block);

        MSGPACK_DEFINE_ARRAY(client_id, time_stamp, block);
    };

    class Message {
    public:
        PBTF_MType mtype;
        uint32_t replica_id;
        std::string pub_key;
        uint64_t seq;
        uint32_t view_id;
        std::string digest;
        std::string signature;
        Block block;


        MSGPACK_DEFINE_ARRAY(mtype, replica_id, pub_key, seq, view_id, digest, signature, block);
    };

    class Proposal {
    public:
        PBFT_PStat stat;
        uint64_t seq;
        Block block;
        uint32_t f;

        Proposal(uint64_t seq, const Block& block, uint32_t f);
    };

    class PBFTHandler : public PublicSingleton<PBFTHandler> {
        friend PublicSingleton<PBFTHandler>;
    public:
        void GetRequest(const Request& req);

        void Preprepare(const Request &req);

    private:
        uint32_t id_;  // 标识PBFT节点
        uint32_t current_primary_;
        std::string priv_key;
        std::string pub_key;
        uint32_t f_;   // 容错F
        uint64_t next_proposal_seq_;   // 下一个提案的序列号
        std::map<uint64_t, Proposal> proposals_;
        std::map<uint64_t, NodeMeta> nodes_;


    };
}


#endif //SIMPLEBITCOIN_BFT_NODE_H
