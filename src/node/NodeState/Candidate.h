//
// Created by nathan on 12/06/15.
//

#ifndef CRAFT_CANDIDATE_H
#define CRAFT_CANDIDATE_H

#include "NodeState.h"


class Candidate : public NodeState {

public:
    Candidate(asio::io_service &io_service, State *s);

    void AppendEntries(uint64_t term, uint64_t leaderId, uint64_t prevLogIndex, std::vector<uint64_t> entries,
                       uint64_t leaderCommit);

    void RequestVote(uint64_t term, uint64_t candidateId, uint64_t lastLogIndex, uint64_t lastLogTerm);
};


#endif //CRAFT_CANDIDATE_H
