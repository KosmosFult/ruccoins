//
// Created by flt on 5/12/24.
//

#ifndef SIMPLEBITCOIN_BFT_NODE_H
#define SIMPLEBITCOIN_BFT_NODE_H

#include <cstdint>
#include <rpc/client.h>
#include "../singleton.h"
//#include "../structure.h"

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
        uint32_t client_id;
        uint64_t time_stamp;
        std::string body;

        Request(uint64_t client_id,
                uint64_t time_stamp,
                std::string  body);

        MSGPACK_DEFINE_ARRAY(client_id, time_stamp, body);
    };

    class Message {
    public:
        PBTF_MType mtype;
        uint32_t replica_id;
        uint32_t client_id;
        std::string pub_key;
        uint64_t seq;         // 唯一标识一个提案
        uint32_t view_id;
        std::string digest;
        std::string signature;
        std::string body;

        Message(PBTF_MType msg_type, uint32_t replica_id, uint32_t client_id, std::string  pub_key, uint64_t seq, uint32_t view_id, std::string body = "");

        MSGPACK_DEFINE_ARRAY(mtype, replica_id, pub_key, seq, view_id, digest, signature, body);
    };

    class Proposal {
    public:
        uint32_t  client_id;
        PBFT_PStat stat;
        uint64_t seq;
        std::string body;
        uint32_t f;
        std::vector<uint32_t> prepare_votes;
        std::vector<uint32_t> commit_votes;

        Proposal(uint32_t client_id, uint64_t seq, std::string body, uint32_t f);
    };

    class PBFTHandler : public PublicSingleton<PBFTHandler> {
        friend PublicSingleton<PBFTHandler>;
    public:

        /**
         * @brief 由client进行rpc调用，接收请求，且自己为primary，调用preprepare
         * @param req
         */
        void GetRequest(const Request& req);

        /**
         * @brief 仅由 GetRequest 调用，将pre-prepare消息广播给所有节点（rpc调用其它节点的prepare）
         * @param req
         */
        void Preprepare(const Request &req);

        /**
         * @brief rpc 调用，检查是否赞成提案，若赞成，广播一个prepare消息（rpc调用其它节点的commit）
         * 若该消息对应的提案没有在提案表中，则添加到提案表
         * @param m，pre-prepare的消息，body为提案原始信息
         */
        void Prepare(Message m);

        /**
         * @brief rpc 调用，收到其它节点的paprare消息，更新对应提案的投票信息prepare_votes，并检查是否达到合法值，若达到则广播commit信息。
         * 若该消息对应的提案没有在提案表中，则添加到提案表
         * @param m，prepare的消息, body为空
         */
        void Commit(Message m);

    private:
        uint32_t id_;  // 标识PBFT节点
        uint32_t current_primary_;
        std::string priv_key;
        std::string pub_key;
        uint32_t f_;   // 容错F
        uint64_t next_proposal_seq_;   // 下一个提案的序列号
        std::map<uint64_t, Proposal> proposals_;
        std::map<uint32_t, NodeMeta> nodes_;


    };
}


#endif //SIMPLEBITCOIN_BFT_NODE_H
