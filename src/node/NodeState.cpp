//
// Created by nathan on 08/06/15.
//

#include "NodeState.h"
#include <asio.hpp>
#include <chrono>


void FollowerState::Followerstate() {

    // TODO: Register async handler for election_timer running out (ie. convert to Candidate
}

void FollowerState::AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                                  std::vector<uint64_t> entries, uint64_t leaderCommit) {

    election_timer_.expires_from_now(std::chrono::milliseconds(Constants::election_timeout));
}

void FollowerState::RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) {
}

