//
// Created by flt on 5/12/24.
//

#include "bft_node.h"

#include <utility>

namespace PBFT{
    Request::Request(uint64_t client_id, uint64_t time_stamp, const Block& block): client_id(client_id), time_stamp(time_stamp)
                                                                                   {
        this->block = block;
    }

    Proposal::Proposal(uint64_t seq, const Block& block, uint32_t f) : stat(PBFT_PStat::pre_prepared), seq(seq), f(f){
        this->block = block
    }

    void PBFTHandler::GetRequest(const Request& req) {
        assert(current_primary_ == id_);
        Preprepare(req);
    }

    void PBFTHandler::Preprepare(const Request &req) {
        uint64_t current_seq = next_proposal_seq_++;

        if(proposals_.find(current_seq) == proposals_.end()){
            auto p = Proposal(current_seq, req.block, f_);
            proposals_.insert(std::make_pair(current_seq, p));
        }

        Message m = {
                PBTF_MType::Preprepare,
                id_,
                pub_key,
                f_,
                0,


        };
    }


}