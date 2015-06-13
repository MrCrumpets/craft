//
// Created by nathan on 08/06/15.
//

#include "NodeState.h"
#include "Follower.h"
#include "Candidate.h"
#include "Leader.h"

State::State(asio::io_service& io_service) : io_service_(io_service) {
    state_ = std::unique_ptr<NodeState>(new Follower(io_service, this));
}

void State::changeState(const States s) {
    switch(s) {
        case States::Follower:
            state_.reset(new Follower(io_service_, this));
            break;
        case States::Candidate:
            state_.reset(new Candidate(io_service_, this));
            break;
        case States::Leader:
            state_.reset(new Leader(io_service_, this));
            break;
    }
}
