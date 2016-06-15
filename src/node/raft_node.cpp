//
// Created by nathan on 08/06/15.
//

#include "raft_node.h"

raft_node::raft_node(const node_id_t &uuid, std::shared_ptr<raft::config> conf) :
        uuid_(uuid),
        rpc_(
                this, std::make_unique<raft_network>(this, io_service_, uuid, conf->getPeers()),
                std::make_unique<state>(io_service_, uuid, conf)
        ) {
    logger_ = spdlog::rotating_logger_st(std::to_string(uuid), "logs/" + std::to_string(uuid), 1024 * 1024 * 5, 3,
                                         true);
    logger_->info("Instantiated node {}", uuid);
}

void raft_node::run() {
    io_service_.run();
}

void raft_node::dispatch_message(std::shared_ptr<raft_message> msg) {
    rpc_.rpc(msg);
}


raft_rpc::raft_rpc(raft_node *context, std::unique_ptr<raft_network> &&network, std::unique_ptr<state> &&state)
        : ctx_(context), network_(std::move(network)), state_(std::move(state)),
          mode_(State::Follower) {
    reset_election_timer(); // kick things off
}

void raft_rpc::rpc(std::shared_ptr<raft_message> msg) {
    if (!msg) return;

    if (msg->term_ > state_->currentTerm_) {
        state_->setTerm(msg->term_);
        changeState(State::Follower);
        return; // don't bother doing anything else with this message
    }

    switch (msg->type_) {
        case MessageType::AppendEntries:
            AppendEntries(std::static_pointer_cast<append_entries>(msg));
            break;
        case MessageType::AppendEntriesResponse:
            break;
        case MessageType::RequestVote:
            RequestVote(std::static_pointer_cast<request_votes>(msg));
            break;
        case MessageType::RequestVoteResponse:
            voteResponse(std::static_pointer_cast<response>(msg));
            break;
    }
}

void raft_rpc::changeState(const State s) {
    switch (s) {
        case State::Follower:
            break;
        case State::Candidate:
            ctx_->logger_->info("Transitioned to Candidate");
            state_->voteFor(state_->getNodeID());


            // Restart election if enough votes aren't captured
            reset_election_timer();

            // Request Votes
            network_->broadcast(std::make_unique<request_votes>(
                    state_->currentTerm_,
                    state_->lastApplied_,
                    state_->entryTerms_.size() ? state_->entryTerms_.back() : 1,
                    state_->getNodeID()
            ));
            break;
        case State::Leader:
            ctx_->logger_->info("Transitioned to Leader");
            ctx_->logger_->info("New Term: {}", state_->currentTerm_);
            heartbeat();
            break;
    }
}


void raft_rpc::reset_election_timer() {
    state_->resetTime(election_timeout, election_timeout + 0.5 * election_timeout);
    state_->election_timer_.async_wait([this](const std::error_code &ec) {
        if(!ec) {
            ctx_->logger_->warn("Election timer ran out. Restarting election.");
            changeState(State::Candidate);
        }
    });
}

void raft_rpc::AppendEntries(std::shared_ptr<append_entries> msg) {
    switch (mode_) {
        case State::Follower:
            ctx_->logger_->info("Received hearbeat from leader {}", msg->leader_id);
            // TODO: reply false if log doesn't contain an entry at prevLogIndex
            if (msg->term_ < state_->currentTerm_) {
                ctx_->logger_->warn("Leader term is behind current term {} < {}", msg->term_, state_->currentTerm_);
                network_->send_to_id(msg->leader_id, std::make_shared<response>(state_->currentTerm_, false));
            }
            // TODO: if an existing entry conflicts with a new one (same index but different terms), delete the existing entry and al that follow it
            // TODO: Append any new entries not already in the log
            // TODO: If leaderCommit > commitIndex, set commitEnd = min(leaderCommit, index of last new entry)
            break;
        case State::Candidate:
            ctx_->logger_->warn("Received heartbeat from {}! terms (me, other): ({}, {})", msg->leader_id,
                                state_->currentTerm_, msg->term_);
            break;
        case State::Leader:
            break;
    }
}

void raft_rpc::RequestVote(std::shared_ptr<request_votes> msg) {
    switch (mode_) {
        case State::Follower:
            ctx_->logger_->info("Vote request from {}", msg->candidate_id);
            if (state_->votedFor_ == 0 && msg->term_ >= state_->currentTerm_) {
                ctx_->logger_->info("Vote cast for {}", msg->candidate_id);
                network_->send_to_id(msg->candidate_id, std::make_shared<response>(state_->currentTerm_, true));
                // Restart election timer TODO: Not sure if this is correct behaviour
                reset_election_timer();
            }
            else {
                ctx_->logger_->info("Vote denied for {}", msg->candidate_id);
                network_->send_to_id(msg->candidate_id, std::make_shared<response>(state_->currentTerm_, false));
            }
            break;
        case State::Candidate:
            ctx_->logger_->info("Vote requested from another candidate but already voted for self");
            network_->send_to_id(msg->candidate_id, std::make_shared<response>(state_->currentTerm_, false));
            break;
        case State::Leader:
            break;
    }
}


void raft_rpc::voteResponse(std::shared_ptr<response> msg) {
    if(msg->success) {
        ctx_->logger_->info("Received vote!");
        // TODO: Increment vote acquired
        uint64_t votes_acquired = 0;
        if(state_->getConf()->isMajority(votes_acquired)) {
            changeState(State::Leader);
        }
    }
    else {
        ctx_->logger_->warn("Vote request denied");
    }
};


void raft_rpc::heartbeat(const std::error_code &ec) {
    ctx_->logger_->info("Sending heartbeat");
    ctx_->logger_->info("{} {} {} {}", state_->currentTerm_, state_->lastApplied_, state_->entryTerms_.back(),
                        state_->uuid);
    network_->broadcast(std::make_unique<append_entries>(
            state_->currentTerm_,
            state_->lastApplied_,
            state_->entryTerms_.back(),
            state_->uuid
    ));
    state_->resetTime(leader_idle_time, leader_idle_time + 1);
    state_->election_timer_.async_wait(std::bind(&raft_rpc::heartbeat, this, std::placeholders::_1));
}
