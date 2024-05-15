//
// Created by flt on 5/12/24.
//

#include "bft_node.h"
#include "../utils.h"
#include <utility>
#include <nlohmann/json.hpp>
#include <chrono>
#include <rpc/rpc_error.h>

using json = nlohmann::json;

namespace PBFT {

    Message::Message(PBFT_MType msg_type, uint32_t replica_id, uint32_t client_id, uint64_t time_stamp,
                     std::string pub_key, uint64_t seq,
                     uint32_t view_id, std::string digest,
                     std::string body) : mtype(msg_type), replica_id(replica_id), client_id(client_id),
                                         time_stamp(time_stamp),
                                         pub_key(std::move(pub_key)), seq(seq),
                                         view_id(view_id), digest(std::move(digest)), body(std::move(body)) {

    }


    Proposal::Proposal(uint32_t time_stamp, uint32_t client_id, uint64_t seq, std::string body, uint32_t f)
            : time_stamp(time_stamp), client_id(client_id),
              stat(PBFT_PStat::pre_prepared),
              seq(seq), f(f),
              body(std::move(body)), prepared(false), committed(false) {
    }

    void Proposal::UpdateInfo(const Message &m) {

        if (m.mtype == PBFT_MType::Prepare) {
            if (prepare_votes.find(m.digest) == prepare_votes.end())
                prepare_votes.insert(std::make_pair(m.digest, std::vector<uint32_t>()));
            prepare_votes[m.digest].push_back(m.replica_id);
        } else if (m.mtype == PBFT_MType::Commit) {
            if (commit_votes.find(m.digest) == commit_votes.end())
                commit_votes.insert(std::make_pair(m.digest, std::vector<uint32_t>()));
            commit_votes[m.digest].push_back(m.replica_id);
        }

    }

    bool Proposal::CheckVotes(PBFT_MType mtype) {

        auto f_ = f;
        if (mtype == PBFT_MType::Prepare) {
            if (prepared)
                return true;
            if (std::any_of(prepare_votes.begin(), prepare_votes.end(),
                            [f_](const auto &p) { return p.second.size() >= (2 * f_ + 1); }))
                return true;
            return false;
        } else if (mtype == PBFT_MType::Commit) {
            if (committed)
                return true;
            if (std::any_of(commit_votes.begin(), commit_votes.end(),
                            [f_](const auto &p) { return p.second.size() >= (2 * f_ + 1); }))
                return true;
            return false;
        }

        return false;
    }

    void PBFTHandler::GetRequest(const Request &req) {
        assert(current_primary_ == id_);
        Preprepare(req);
//        std::cout << req.body << std::endl;
    }

    void PBFTHandler::Preprepare(const Request &req) {
        uint64_t current_seq = next_proposal_seq_++;

        console_logger_->info("Preprepare seq: {}", current_seq);

        if (proposals_.find(current_seq) == proposals_.end()) {
            auto p = std::make_shared<Proposal>(req.time_stamp, req.client_id, current_seq, req.body, f_);
            proposals_.insert(std::make_pair(current_seq, p));
        }

        Message m(PBFT_MType::Preprepare, id_, req.client_id, req.time_stamp, pub_key, current_seq, 0,
                  Message::GetDigest(req.body), req.body);
        Message::Signate(m, priv_key);
        assert(Message::CheckSignature(m));
        SendMessage(m);
    }

    void PBFTHandler::Init(uint32_t id, const std::string &config_path) {
        if (inited_) {
            std::cerr << "Has been Inited" << std::endl;
            return;
        }
        id_ = id;
        std::fstream f(config_path);
        if (!f.is_open()) {
            std::cerr << "Can not open file " << "\"" << config_path << "\"" << std::endl;
            return;
        }

        json conf_json = json::parse(f);

        uint32_t index = 0;
        for (auto &node_addr: conf_json["node_addr"]) {
            auto ap = ParseAddr(node_addr);
            NodeMeta nmeta = {
                    index,
                    ap.first,
                    ap.second + 1000,
                    conf_json[std::to_string(ap.second)]["addr"]
            };
            nodes_.insert(std::make_pair(index, nmeta));

            if (index == id) {
                pub_key = conf_json[std::to_string(ap.second)]["addr"];
                priv_key = conf_json[std::to_string(ap.second)]["priv_key"];
                master_name_ = std::make_pair("127.0.0.1", ap.second);

            }

            index++;
        }

        next_proposal_seq_ = 0;
        f_ = (nodes_.size() - 1) / 3;

        console_logger_ = spdlog::stdout_color_mt("console");
        std::string pattern = "[PBFT NODE " + std::to_string(id_) + "] %v";
        console_logger_->set_pattern(pattern);
        error_logger_ = spdlog::stderr_color_mt("stderr");
        error_logger_->set_pattern(pattern);
        inited_ = true;
    }


    std::pair<std::string, uint32_t> PBFTHandler::ParseAddr(const std::string &addr) {
        auto flag = addr.find(':');
        std::string node_ip = addr.substr(0, flag);
        int node_port = atoi(addr.substr(flag + 1, addr.length() - flag).data());
        return std::make_pair(node_ip, node_port);
    }

    void PBFTHandler::SendMessage(const Message &m) {
        std::vector<std::shared_ptr<rpc::client>> connects;
        std::vector<std::future<clmdep_msgpack::object_handle>> futures;

        for (auto &node: nodes_) {

            auto cl = std::make_shared<rpc::client>(node.second.hostname, node.second.port);
            connects.push_back(cl);
            switch (m.mtype) {
                case PBFT_MType::Preprepare:
                    futures.push_back(cl->async_call("Prepare", m));
                    break;
                case PBFT_MType::Prepare:
                    futures.push_back(cl->async_call("Commit", m));
                    break;
                default:
                    break;
            }
        }

        for (auto &f: futures) {
            if (f.wait_for(std::chrono::milliseconds(system_value::timeout)) == std::future_status::timeout) {
                error_logger_->info("Send message time out");
            } else {
//                    f.get();
            }
        }

        console_logger_->info("exit send message");

    }


    void PBFTHandler::Prepare(const Message &m) {
        if (!Message::CheckSignature(m))
            return;

        if (!CheckSeqNumber(m))
            return;

        if (proposals_.find(m.seq) == proposals_.end()) {
            auto p = std::make_shared<Proposal>(m.time_stamp, m.client_id, m.seq, m.body, f_);
            proposals_.insert(std::make_pair(m.seq, p));
        }

        if (!CheckProposal(proposals_[m.seq]))
            return;

        console_logger_->info("(Prepare), seq:{}, m:{}", m.seq, m.body);

        Message sm(PBFT_MType::Prepare, id_, m.client_id, m.time_stamp, pub_key, m.seq, 0, Message::GetDigest(m.body));
        Message::Signate(sm, priv_key);
        assert(Message::CheckSignature(sm));
        SendMessage(sm);
    }

    void PBFTHandler::Commit(const Message &m) {
        if (!Message::CheckSignature(m))
            return;
        if (!CheckSeqNumber(m))
            return;


        if (proposals_.find(m.seq) == proposals_.end()) {
            auto p = std::make_shared<Proposal>(m.time_stamp, m.client_id, m.seq, m.body, f_);
            proposals_.insert(std::make_pair(m.seq, p));
        }

        auto &p = proposals_[m.seq];
        p->UpdateInfo(m);
        if (p->CheckVotes(m.mtype)) {
            p->prepared = true;
            console_logger_->info("Commit seq:{}", m.seq);
        }
    }


    bool PBFTHandler::CheckProposal(const std::shared_ptr<Proposal> &p) {
        return true;
    }

    bool PBFTHandler::CheckSeqNumber(const Message &m) {
        return true;
    }


    void Message::Signate(Message &m, std::string &priv_key) {
        EC_KEY *privateKey = hexToPrivateKey(priv_key); // 使用之前定义的从Hex转换私钥的函数
        if (!privateKey) {
            std::cerr << "Error creating private key from hex." << std::endl;
            return;
        }


        std::stringstream ss;
        ss << ToInt(m.mtype) << m.view_id << m.seq << m.digest;

        std::string sig_hash = ss.str();
        unsigned char *signature = (unsigned char *) malloc(ECDSA_size(privateKey));
        unsigned int sig_len = 0;

        if (!ECDSA_sign(0, reinterpret_cast<const unsigned char *>(sig_hash.c_str()), sig_hash.length(), signature,
                        &sig_len, privateKey)) {
            std::cerr << "Failed to sign transaction. Error code: " << ERR_get_error() << std::endl;
            EC_KEY_free(privateKey);
            free(signature);
            return;
        }

        std::string signatureHex;
        for (unsigned int i = 0; i < sig_len; i++) {
            char hex[3];
            sprintf(hex, "%02x", signature[i]);
            signatureHex.append(hex);
        }

        EC_KEY_free(privateKey);
        free(signature);
        m.signature = signatureHex;
        return;
    }

    std::string Message::GetDigest(const std::string &data) {
        return RuccoinHash(data);
    }

    int Message::ToInt(PBFT_MType type) {
        switch (type) {
            case PBFT_MType::Preprepare:
                return 1;
            case PBFT_MType::Prepare:
                return 2;
            case PBFT_MType::Commit:
                return 3;
            default:
                throw std::invalid_argument("Unknown PBFT message type");
        }

    }

    bool Message::CheckSignature(const Message &m) {
        std::string pubHex = m.pub_key;
        EC_KEY *publicKey = hexToPublicKey(pubHex); // Hex转换公钥
        if (!publicKey) {
            std::cerr << "Error creating public key from hex." << std::endl;
            return false;
        }

        std::stringstream ss;
        ss << ToInt(m.mtype) << m.view_id << m.seq << m.digest;
        std::string sig_hash = ss.str();

        std::vector<unsigned char> signature;
        size_t sig_len = m.signature.length() / 2;
        signature.reserve(sig_len);
        //解析出签名数据。这个字节序列是签名的实际二进制表示。
        for (size_t i = 0; i < sig_len; i++) {
            unsigned int byte;
            sscanf(m.signature.c_str() + 2 * i, "%02x", &byte);
            signature.push_back(static_cast<unsigned char>(byte));
        }

        // 验证签名
        int verify_status = ECDSA_verify(0, (unsigned char *) sig_hash.c_str(), sig_hash.length(), signature.data(),
                                         signature.size(), publicKey);
        if (verify_status != 1) {
            std::cerr << "Signature verification failed." << std::endl;
            EC_KEY_free(publicKey);
            return false;
        }

        EC_KEY_free(publicKey);
        return true;
    }

}