//
// Created by flt on 5/12/24.
//

#ifndef SIMPLEBITCOIN_BFT_NODE_H
#define SIMPLEBITCOIN_BFT_NODE_H

#include <cstdint>
#include <rpc/client.h>
#include "../singleton.h"
#include <memory>
#include "./include/bft.h"
#include "spdlog/spdlog.h"
#include "spdlog/pattern_formatter.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include <mutex>
//#include "../structure.h"

enum class PBFT_MType {
    Preprepare,
    Prepare,
    Commit
};

enum class PBFT_PStat {
    pre_prepared, prepared, committed
};

MSGPACK_ADD_ENUM(PBFT_MType);
MSGPACK_ADD_ENUM(PBFT_PStat);

namespace PBFT {


    namespace system_value {
        const uint32_t timeout = 8000;
    }

    class Message;

    class Proposal;

    struct NodeMeta {
        uint32_t replica_id;
        std::string hostname;
        uint32_t port;
        std::string pub_key;
    };


    class Message {
    public:
        PBFT_MType mtype;
        uint32_t replica_id;
        uint32_t client_id;
        uint64_t time_stamp;
        std::string pub_key;
        uint64_t seq;         // 唯一标识一个提案
        uint32_t view_id;
        std::string digest;
        std::string signature;
        std::string body;

        static int ToInt(PBFT_MType type);

        static std::string MTypeStr(PBFT_MType mtype);

        static std::string GetDigest(const std::string &data);


        /**
         * @brief 计算签名的数据
         * @param m
         * @param priv_key
         */
        static void Signate(Message &m, std::string &priv_key);

        static bool CheckSignature(const Message &m);


        Message(){}

        Message(PBFT_MType msg_type, uint32_t replica_id, uint32_t client_id, uint64_t time_stamp, std::string pub_key,
                uint64_t seq, uint32_t view_id, std::string digest, std::string body = "");

        MSGPACK_DEFINE_ARRAY(mtype, replica_id, client_id, time_stamp, pub_key, seq, view_id, digest, signature, body);
    };

    class Proposal {
    public:
        uint32_t client_id;
        PBFT_PStat stat;
        uint64_t seq;
        uint64_t time_stamp;   // 提案提出时间
        std::string body;
        uint32_t f;
        bool prepared;
        bool committed;
        std::mutex ins_mutex_;
        std::condition_variable committed_cv;
        // key为digest，value是投票。因为可能消息被篡改，因此需要通过digest区别投票
//        std::map<std::string, std::string> message;  // digest到原始message
        std::map<std::string, std::vector<uint32_t>> prepare_votes;
        std::map<std::string, std::vector<uint32_t>> commit_votes;

        Proposal(uint32_t time_stamp, uint32_t client_id, uint64_t seq, std::string body, uint32_t f);

        void UpdateInfo(const Message& m);

        bool CheckVotes(PBFT_MType mtype);
    };

    class PBFTHandler : public PublicSingleton<PBFTHandler> {
        friend PublicSingleton<PBFTHandler>;
    public:
        PBFTHandler(const PBFTHandler &) = delete;

        PBFTHandler &operator=(const PBFTHandler &) = delete;

        PBFTHandler() : inited_(false), check_proposal_call_name_("CheckProposal"), commit_proposal_call_name_("CommitProposal") {}

        void Init(uint32_t id, const std::string &config_json_path);


        /**
         * @brief 由client进行rpc调用，接收请求，且自己为primary，调用preprepare
         * @param req
         */
        void GetRequest(const Request &req);

        /**
         * @brief 仅由 GetRequest 调用，将pre-prepare消息广播给所有节点（rpc调用其它节点的prepare）
         * @param req
         */
        uint64_t Preprepare(const Request &req);

        /**
         * @brief rpc 调用，检查是否赞成提案，若赞成，广播一个prepare消息（rpc调用其它节点的commit）
         * 若该消息对应的提案没有在提案表中，则添加到提案表
         * @param m，pre-prepare的消息，body为提案原始信息
         */
        void Prepare(const Message& m);

        /**
         * @brief rpc 调用，收到其它节点的paprare消息，更新对应提案的投票信息prepare_votes，并检查是否达到合法值，若达到则广播commit信息。
         * 若该消息对应的提案没有在提案表中，则添加到提案表
         * @param m，prepare的消息, body为空
         */
        void Commit(const Message& m);

        void Reply(const Message& m);

        uint32_t GetPort() { return nodes_[id_].port; }

    private:
        bool inited_;

        // master ralated
        std::pair<std::string, uint32_t> master_name_;   // 使用该pbft的主节点，主要用于检查proposal
        std::string check_proposal_call_name_;            // master中实现检查proposal的rpc调用函数名，默认为 "CheckProposal"
        std::string commit_proposal_call_name_;           // master中实现检查proposal的rpc调用函数名，默认为 "CommitProposal"

        uint32_t id_;  // 标识PBFT节点，这里定为master port - 8090
        uint32_t current_primary_;
        std::string priv_key;
        std::string pub_key;
        uint32_t f_;   // 容错F
        uint64_t next_proposal_seq_;   // 下一个提案的序列号
        std::mutex seq_lock_;
        std::map<uint64_t, std::shared_ptr<Proposal>> proposals_;
        std::mutex proposals_mutex;
        std::map<uint32_t, NodeMeta> nodes_;

        std::shared_ptr<spdlog::logger> console_logger_;
        std::shared_ptr<spdlog::logger> error_logger_;

        static std::pair<std::string, uint32_t> ParseAddr(const std::string &addr);

        void SendMessage(const Message &m);

        /**
         * @brief 与master通信，检查m是否应该同意，master需要实现 `check_proposal_call_name_` 的rpc调用
         * @param p 需要检查的proposal
         * @return
         */
        bool CheckProposal(const std::shared_ptr<Proposal>& p);

        bool CheckSeqNumber(const Message &m);

        /**
         * @brief 具体的提交操作，master需要实现 `commit_proposal_call_name_` 的rpc调用
         * @param m
         */
        void CommitAction(std::string& m);


    };
}


#endif //SIMPLEBITCOIN_BFT_NODE_H
