//
// Created by flt on 5/12/24.
//

#include "bft_node.h"
#include "../utils.h"
#include <utility>

namespace PBFT {

    Message::Message(PBTF_MType msg_type, uint32_t replica_id, uint32_t client_id, std::string pub_key, uint64_t seq,
                     uint32_t view_id,
                     std::string body) : mtype(msg_type), replica_id(replica_id), client_id(client_id),
                                         pub_key(std::move(pub_key)), seq(seq),
                                         view_id(view_id), body(std::move(body)) {

    }


    Request::Request(uint64_t client_id, uint64_t time_stamp, std::string body) : client_id(client_id),
                                                                                  time_stamp(time_stamp),
                                                                                  body(std::move(body)) {
    }

    Proposal::Proposal(uint32_t client_id, uint64_t seq, std::string body, uint32_t f) : client_id(client_id),
                                                                                         stat(PBFT_PStat::pre_prepared),
                                                                                         seq(seq), f(f),
                                                                                         body(std::move(body)) {
    }

    void PBFTHandler::GetRequest(const Request& req) {
        assert(current_primary_ == id_);
        Preprepare(req);
    }

    void PBFTHandler::Preprepare(const Request &req) {
        uint64_t current_seq = next_proposal_seq_++;

        if (proposals_.find(current_seq) == proposals_.end()) {
            auto p = Proposal(req.client_id, current_seq, req.body, f_);
            proposals_.insert(std::make_pair(current_seq, p));
        }

    }

}