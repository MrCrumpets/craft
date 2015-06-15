//
// Created by nathan on 12/06/15.
//

#include "Candidate.h"

Candidate::Candidate(asio::io_service &io_service, State *s) : NodeState(io_service, s) {
    std::cout << "I'm a candidate now!" << std::endl;
    // TODO: Increment current term
    // TODO: Vote for self
    // TODO: Reset election timer
    // TODO: Send RequestVote RPC's to all other servers (need mechanism for sending messages to all servers)
    // TODO: If votes received from majority of servers: become leader
    // TODO: If AppendEntries RPC received from new leader: convert to follower
    // TODO: If election timeout elapses: start new election
}

void Candidate::AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex,
                              std::vector<uint64_t> entries, uint64_t leaderCommit) {

}

void Candidate::RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm) {
}

