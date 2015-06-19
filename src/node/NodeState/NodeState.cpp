//
// Created by nathan on 08/06/15.
//

#include "NodeState.h"
#include "Follower.h"
#include "Candidate.h"
#include "Leader.h"

State::State(const std::string &address, const short in_port, const std::vector<std::pair<short, std::string>> &peers) : peers_(peers){
    state_ = std::unique_ptr<NodeState>(new Follower(io_service_, this));
}

void State::run() {
    io_service_.run();
}

void State::changeState(const States s) {
    switch(s) {
        case States::Follower:
            state_.reset(new Follower(io_service_, this));
            break;
        case States::Candidate:
            state_.reset(new Candidate(io_service_, this));
            break;
    }
}

