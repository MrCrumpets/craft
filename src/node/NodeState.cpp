//
// Created by nathan on 08/06/15.
//

#include "NodeState.h"
#include <asio.hpp>


Follower::Follower(asio::io_service &io_service) : NodeState(io_service), election_timer_(io_service) {
    // TODO: Register async handler for election_timer running out (ie. convert to Candidate
    election_timer_.expires_from_now(std::chrono::milliseconds(Constants::election_timeout));
    election_timer_.async_wait([this](const std::system_error &ec) {
        std::cout << "Election time!" << std::endl;
    });
}

void Follower::AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                                  std::vector<uint64_t> entries, uint64_t leaderCommit) {

    election_timer_.expires_from_now(std::chrono::milliseconds(Constants::election_timeout));
}

void Follower::RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) {
}

