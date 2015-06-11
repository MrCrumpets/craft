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

class NodeState;

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

    std::unique_ptr<NodeState> state_;
    asio::io_service &io_service_;

public:
    State(asio::io_service& io_service) : io_service_(io_service) {}
};

class NodeState {
protected:
    std::unique_ptr<State> ctx_;
    asio::io_service &io_service_;

public:
    virtual void AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                               std::vector<uint64_t> entries, uint64_t leaderCommit) = 0;

    virtual void RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) = 0;

    NodeState(asio::io_service& io_service) : io_service_(io_service) {}
};

class Follower : public NodeState {
public:
    Follower(asio::io_service &io_service);

    void AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                               std::vector<uint64_t> entries, uint64_t leaderCommit);

    void RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm);

    asio::steady_timer election_timer_;
};

class Candidate : public NodeState {

public:
    Candidate(asio::io_service &io_service) : NodeState(io_service) { }
};
class Leader : public NodeState {
public:
    Leader(asio::io_service &io_service) : NodeState(io_service) { }

private:
// Volatile (leader state)
    std::vector<uint64_t> nextIndex_;
    std::vector<uint64_t> matchIndex_;
};


#endif //CRAFT_NODESTATE_H
