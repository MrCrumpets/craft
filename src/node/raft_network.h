//
// Created by nathan on 08/06/15.
//

#ifndef CRAFT_CONNECTION_H
#define CRAFT_CONNECTION_H

#include <asio/streambuf.hpp>
#include <asio/io_service.hpp>
#include <asio/buffer.hpp>
#include <asio/ip/udp.hpp>
#include <asio/steady_timer.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/base_class.hpp>
#include <map>
#include <stdint.h>
#include "state.h"

using asio::ip::udp;

class raft_node;

struct raft_node_endpoint_t {
    node_id_t uuid;
    std::string addr;
    short out_port;
    short in_port;
};

enum class MessageType {
    AppendEntries,
    AppendEntriesResponse,
    RequestVote,
    RequestVoteResponse
};

struct raft_message {
    MessageType type_;
    uint64_t term_;
    raft_message() {}

    virtual void sayType() {}
    template<class Archive>
    void serialize(Archive &archive) {
        archive(CEREAL_NVP(type_));
    }

    raft_message(MessageType type, uint64_t term) : type_(type), term_(term) { }
};

struct append_entries : public raft_message {
    uint64_t prev_log_entry, prev_log_term;
    std::vector<uint64_t> entries;
    node_id_t leader_id;
    void n() {}
    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::base_class<raft_message>(this), prev_log_entry, prev_log_term, CEREAL_NVP(entries), leader_id);
    }

    append_entries() : raft_message(MessageType::AppendEntries, 0) { }

    append_entries(uint64_t term, uint64_t pli, uint64_t plt, node_id_t lid) :
            raft_message(MessageType::AppendEntries, term),
        prev_log_entry(pli),
        prev_log_term(plt),
        leader_id(lid) {}
};

struct request_votes : public raft_message {
    uint64_t last_log_index, last_log_term;
    node_id_t candidate_id;
    void n() {}
    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::base_class<raft_message>(this), last_log_index, last_log_term, candidate_id);
    }

    request_votes() : raft_message(MessageType::RequestVote, 0) { }

    request_votes(uint64_t term, uint64_t lli, uint64_t llt, node_id_t cid) :
            raft_message(MessageType::RequestVote, term),
            last_log_index(lli),
            last_log_term(llt),
            candidate_id(cid) {}
};

struct response : public raft_message {
    bool success;
    void n() {}
    template<class Archive>
    void serialize(Archive &archive) {
        archive(cereal::base_class<raft_message>(this), success);
    }

    response() : raft_message(MessageType::RequestVoteResponse, 0) { };
    response(uint64_t term, bool success) :
            raft_message(MessageType::RequestVoteResponse, term),
            success(success) {};
};

#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>
CEREAL_REGISTER_TYPE(append_entries);
CEREAL_REGISTER_TYPE(request_votes);
CEREAL_REGISTER_TYPE(response);

class connection {
    std::shared_ptr<udp::socket> socket_;
    udp::endpoint remote_endpoint_;
    std::array<char, 1024> in_buffer_;
    raft_node *node_;

    void do_receive();

public:
    connection(raft_node* node, std::shared_ptr<udp::socket> socket, udp::endpoint endpoint);

    void do_send(std::shared_ptr<raft_message> m);

};

class raft_network {
    std::map<node_id_t, std::unique_ptr<connection>> connections_;
public:
    raft_network(raft_node *node, asio::io_service &io, const node_id_t &uuid,
                 const std::vector<raft_node_endpoint_t> &peers);

    void broadcast(std::shared_ptr<raft_message>);

    void send_to_id(node_id_t id, std::shared_ptr<raft_message> msg);

};

#endif //CRAFT_CONNECTION_H
