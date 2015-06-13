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

    election_timer_.expires_from_now(std::chrono::milliseconds(election_timeout));
}

void Follower::RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) {
}
