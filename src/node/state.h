//
// Created by nathan on 24/09/15.
//

#ifndef CRAFT_STATE_H
#define CRAFT_STATE_H

#include <stdint.h>
#include <random>
#include <vector>
#include <asio/steady_timer.hpp>

struct raft_node_endpoint_t;

typedef std::uint64_t node_id_t;


namespace raft {
    class config {
        std::vector<raft_node_endpoint_t> peers_;
    public:
        config(const std::vector<raft_node_endpoint_t> peers) : peers_(peers) {}
        const auto& getPeers() { return peers_; }
        bool isMajority(int numVotes) const {
            return numVotes > (peers_.size() - 1) / 2;
        }
    };
}

struct state {
    // Persistent (all states)
    uint64_t currentTerm_;
    uint64_t votedFor_;
    std::vector<uint64_t> logEntries_;
    std::vector<uint64_t> entryTerms_;

    // Volatile (all states)
    node_id_t uuid;
    uint64_t commitIndex_; // index of highest log entry committed
    uint64_t lastApplied_; // index of highest log entry applied
    asio::steady_timer election_timer_;

    // Misc
    std::mt19937 mt;
    std::shared_ptr<raft::config> config_;

    state(asio::io_service &io_service, node_id_t uuid, std::shared_ptr<raft::config> config) :
            election_timer_(io_service), config_(config), uuid(uuid) {
        std::random_device rd;
        mt.seed(rd());
        entryTerms_.push_back(1);
    }
    void incrementTerm() { currentTerm_++; }
    void setTerm(uint64_t term) { currentTerm_ = term; }

    node_id_t getNodeID() { return uuid; }
    void voteFor(uint64_t id) { votedFor_ = id; }
    const raft::config* getConf() { return config_.get(); }
    void resetTime(int a, int b) {
        std::uniform_int_distribution<int> dist(a, b);
        election_timer_.expires_from_now(std::chrono::milliseconds(dist(mt)));
    }
};

#endif //CRAFT_STATE_H
