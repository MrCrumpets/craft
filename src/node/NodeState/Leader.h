//
// Created by nathan on 12/06/15.
//

#ifndef CRAFT_LEADER_H
#define CRAFT_LEADER_H

#include "NodeState.h"

class Leader : public NodeState {
public:
    Leader(asio::io_service &io_service, State *s) : NodeState(io_service, s) { }

private:
// Volatile (leader state)
    std::vector<uint64_t> nextIndex_;
    std::vector<uint64_t> matchIndex_;

    void AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex, std::vector<uint64_t> entries,
                       uint64_t leaderCommit);

    void RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm);
};


#endif //CRAFT_LEADER_H
