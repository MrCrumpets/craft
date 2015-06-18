//
// Created by nathan on 08/06/15.
//

#ifndef CRAFT_NODESTATE_H
#define CRAFT_NODESTATE_H

#include <stdint.h>
#include <vector>
#include <memory>
#include <asio/deadline_timer.hpp>
#include <asio/basic_waitable_timer.hpp>
#include <asio/steady_timer.hpp>
#include "../Connection.h"

class NodeState;

enum class States {
    Follower, Candidate, Leader
};

enum Constants {
    election_timeout = 500
};

class State {

    // Persistent (all states)
    uint64_t currentTerm_;
    uint64_t votedFor_;
    std::vector<uint64_t> log_;

    // Volatile (all states)
    uint64_t commitIndex_; // index of highest log entry committed
    uint64_t lastApplied_; // index of highest log entry applied
    asio::io_service io_service_;
    std::unique_ptr<NodeState> state_;
    std::shared_ptr<Network::Connection> connection_;

public:
    State(const State&) = delete;
    State& operator=(const State&) = delete;

    State(const std::string &address, const short in_port, const std::vector<short> &ports);

    void changeState(States s);
    void incrementTerm();

    void run();
};

class NodeState {
protected:
    State* ctx_;
    asio::io_service &io_service_;

public:
    virtual void AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                               std::vector<uint64_t> entries, uint64_t leaderCommit) = 0;

    virtual void RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) = 0;

    NodeState(asio::io_service &io_service, State *s) : io_service_(io_service), ctx_(s) {}

};



#endif //CRAFT_NODESTATE_H