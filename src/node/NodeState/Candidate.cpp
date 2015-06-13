//
// Created by nathan on 12/06/15.
//

#include "Candidate.h"

Candidate::Candidate(asio::io_service &io_service) : NodeState(io_service) {
    std::cout << "I'm a candidate now!" << std::endl;
}

void Candidate::AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                              std::vector<uint64_t> entries, uint64_t leaderCommit) {

}

void Candidate::RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) {
}

