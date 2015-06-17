//
// Created by nathan on 08/06/15.
//

#include "NodeState.h"
#include "Follower.h"
#include "Candidate.h"
#include "Leader.h"

State::State(const std::string &address, short in_port, short out_port) {
    state_ = std::unique_ptr<NodeState>(new Follower(io_service_, this));
}

void State::run() {
    io_service_.run();
}

void State::incrementTerm() {
    currentTerm_++;
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
