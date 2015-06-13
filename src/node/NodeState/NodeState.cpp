//
// Created by nathan on 08/06/15.
//

#include "NodeState.h"
#include "Follower.h"
#include <asio.hpp>


State::State(asio::io_service& io_service) : io_service_(io_service) {
    state_ = std::unique_ptr<NodeState>(new Follower(io_service));
}

void Candidate::AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                             std::vector<uint64_t> entries, uint64_t leaderCommit) {

}

void Candidate::RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) {
}

void Leader::AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                              std::vector<uint64_t> entries, uint64_t leaderCommit) {

}

void Leader::RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) {
}

void State::changeState(const States s) {
    switch(s) {
        case States::Follower:
            state_.reset(new Follower(io_service_));
            break;
        case States::Candidate:
            state_.reset(new Candidate(io_service_));
            break;
        case States::Leader:
            state_.reset(new Candidate(io_service_));
            break;
    }
}
