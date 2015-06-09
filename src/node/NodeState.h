//
// Created by nathan on 08/06/15.
//

#ifndef CRAFT_NODESTATE_H
#define CRAFT_NODESTATE_H

#include <stdint.h>
#include <vector>
#include <memory>

class NodeState;

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
};

class FollowerState : public NodeState {};
class CandidateState : public NodeState {};
class LeaderState : public NodeState {
    // Volatile (leader state)
    std::vector<uint64_t> nextIndex_;
    std::vector<uint64_t> matchIndex_;
};


#endif //CRAFT_NODESTATE_H
