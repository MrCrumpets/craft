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

class StateContext {
    // Persistent (all states)
    uint64_t currentTerm_;
    uint64_t votedFor_;
    std::vector<uint64_t> log_;

    // Volatile (all states)
    uint64_t commitIndex_; // index of highest log entry committed
    uint64_t lastApplied_; // index of highest log entry applied

    std::unique_ptr<NodeState> state_;
};

class NodeState {
    std::unique_ptr<StateContext> ctx_;
public:
    virtual void AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                               std::vector<uint64_t> entries, uint64_t leaderCommit) = 0;

    virtual void RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) = 0;
};

class FollowerState : public NodeState {
    void AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                               std::vector<uint64_t> entries, uint64_t leaderCommit);

    void RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm);

    void Followerstate();

    asio::steady_timer election_timer_;
};

class CandidateState : public NodeState {};
class LeaderState : public NodeState {
    // Volatile (leader state)
    std::vector<uint64_t> nextIndex_;
    std::vector<uint64_t> matchIndex_;
};


#endif //CRAFT_NODESTATE_H
