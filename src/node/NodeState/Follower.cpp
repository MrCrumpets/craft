//
// Created by nathan on 12/06/15.
//

#include "Follower.h"

Follower::Follower(asio::io_service &io_service, State *s) : NodeState(io_service, s), election_timer_(io_service) {
    election_timer_.expires_from_now(std::chrono::milliseconds(election_timeout));
    election_timer_.async_wait([this](const std::system_error &ec) {
        std::cout << "Election time!" << std::endl;
        ctx_->changeState(States::Candidate);
    });
}

void Follower::AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                             std::vector<uint64_t> entries, uint64_t leaderCommit) {
    // TODO: reply false if term < currentTerm
    // TODO: reply false if log doesn't contain an entry at prevLogIndex
    // TODO: if an existing entry conflicts with a new one (same index but different terms), delete the existing entry and al that follow it
    // TODO: Append any new entries not already in the log
    // TODO: If leaderCommit > commitIndex, set commitEnd = min(leaderCommit, index of last new entry)
    election_timer_.expires_from_now(std::chrono::milliseconds(election_timeout));
}

void Follower::RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) {
}
