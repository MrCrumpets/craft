//
// Created by nathan on 12/06/15.
//

#ifndef CRAFT_FOLLOWER_H
#define CRAFT_FOLLOWER_H

#include "NodeState.h"

class Follower : public NodeState {
public:
    Follower(asio::io_service &io_service);

    void AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                       std::vector<uint64_t> entries, uint64_t leaderCommit);

    void RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm);

    asio::steady_timer election_timer_;
};


#include <asio.hpp>
#include "NodeState.h"

#endif //CRAFT_FOLLOWER_H
