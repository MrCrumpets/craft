//
// Created by nathan on 08/06/15.
//

#include "raft_node.h"

raft_node::raft_node(const node_id_t &uuid, std::shared_ptr<raft::config> conf) :
        uuid_(uuid) {
    logger_ = spdlog::rotating_logger_st(std::to_string(uuid), "logs/" + std::to_string(uuid), 1024 * 1024 * 5, 3,
                                         true);
    logger_->info("Instantiated node {}", uuid);
    mode_ = std::unique_ptr<raft_rpc>(
            new follower_rpc(
                    this, std::make_unique<raft_network>(this, io_service_, uuid, conf->getPeers()),
                    std::make_unique<state>(io_service_, uuid, conf)
            ));
}

void raft_node::run() {
    io_service_.run();
}

void raft_node::changeState(const States s) {
    switch(s) {
        case States::Follower:
            mode_.reset(new follower_rpc(this, mode_->getNetwork(), mode_->getState()));
            break;
        case States::Candidate:
            mode_.reset(new candidate_rpc(this, mode_->getNetwork(), mode_->getState()));
            break;
        case States::Leader:
            mode_.reset(new leader_rpc(this, mode_->getNetwork(), mode_->getState()));
            break;
    }
}

void raft_node::dispatch_message(std::shared_ptr<raft_message> msg) const {
    if(!msg) return;
    switch(msg->type_) {
        case MessageType::AppendEntries:
            mode_->AppendEntries(std::static_pointer_cast<append_entries>(msg));
            break;
        case MessageType::AppendEntriesResponse:
            break;
        case MessageType::RequestVote:
            mode_->RequestVote(std::static_pointer_cast<request_votes>(msg));
            break;
        case MessageType::RequestVoteResponse:
            mode_->voteResponse(std::static_pointer_cast<response>(msg));
            break;
    }
}

void reset_election_timer(raft_node *ctx, state *s) {
    s->resetTime(election_timeout, election_timeout + 0.5 * election_timeout);
    s->election_timer_.async_wait([ctx](const std::error_code &ec) {
        if(!ec) {
            ctx->logger_->warn("Election timer ran out. Restarting election.");
            ctx->changeState(States::Candidate);
        }
    });
}

follower_rpc::follower_rpc(raft_node *ctx, std::unique_ptr<raft_network> &&network, std::unique_ptr<state> &&state)
        : raft_rpc(ctx, std::move(network), std::move(state)) {
    ctx_->logger_->info("Transitioned to Follower");
    reset_election_timer(ctx, state_.get());
}

void follower_rpc::AppendEntries(std::shared_ptr<append_entries> msg) {
    ctx_->logger_->info("Received hearbeat from leader {}", msg->leader_id);
    // TODO: reply false if log doesn't contain an entry at prevLogIndex
    if(msg->leader_term < state_->currentTerm_) {
        ctx_->logger_->warn("Leader term is behind current term {} < {}", msg->leader_term, state_->currentTerm_);
        network_->send_to_id(msg->leader_id, std::make_shared<response>(state_->currentTerm_, false));
    }
    else {
        state_->setTerm(msg->leader_term);
    }
    // TODO: if an existing entry conflicts with a new one (same index but different terms), delete the existing entry and al that follow it
    // TODO: Append any new entries not already in the log
    // TODO: If leaderCommit > commitIndex, set commitEnd = min(leaderCommit, index of last new entry)
}

void follower_rpc::RequestVote(std::shared_ptr<request_votes> msg) {
    ctx_->logger_->info("Vote request from {}", msg->candidate_id);
    if(state_->votedFor_ == 0 && msg->candidate_term >= state_->currentTerm_) {
        ctx_->logger_->info("Vote cast for {}", msg->candidate_id);
        network_->send_to_id(msg->candidate_id, std::make_shared<response>(state_->currentTerm_, true));
        // Restart election timer TODO: Not sure if this is correct behaviour
        reset_election_timer(ctx_, state_.get());
    }
    else {
        ctx_->logger_->info("Vote denied for {}", msg->candidate_id);
        network_->send_to_id(msg->candidate_id, std::make_shared<response>(state_->currentTerm_, false));
    }
}

candidate_rpc::candidate_rpc(raft_node *ctx, std::unique_ptr<raft_network> &&network, std::unique_ptr<state>&& state)
        : raft_rpc(ctx, std::move(network), std::move(state)) {
    ctx_->logger_->info("Transitioned to Candidate");
    state_->incrementTerm();
    state_->voteFor(state_->getNodeID());


    // Restart election if enough votes aren't captured
    reset_election_timer(ctx, state_.get());

    // Request Votes
    network_->broadcast(std::make_unique<request_votes>(
            state_->currentTerm_,
            state_->lastApplied_,
            state_->entryTerms_.size()  ? state_->entryTerms_.back() : 1,
            state_->getNodeID()
    ));

    // TODO: If AppendEntries RPC received from new leader: convert to follower
    // TODO: If election timeout elapses: start new election
}

void candidate_rpc::AppendEntries(std::shared_ptr<append_entries> msg) {
    ctx_->logger_->warn("Received heartbeat from {}! terms (me, other): ({}, {})", msg->leader_id, state_->currentTerm_,
                        msg->leader_term);
    if(msg->leader_term >= state_->currentTerm_) {
        state_->setTerm(msg->leader_term);
        ctx_->changeState(States::Follower);
    }
}

void candidate_rpc::RequestVote(std::shared_ptr<request_votes> msg) {
    // TODO: Already voted for self. Should return false.
    ctx_->logger_->info("Vote requested from Candidate");
    network_->send_to_id(msg->candidate_id, std::make_shared<response>(state_->currentTerm_, false));
}

void candidate_rpc::voteResponse(std::shared_ptr<response> msg) {
    if(msg->success) {
        ctx_->logger_->info("Received vote!");
        votes_acquired.fetch_add(1);
        if(state_->getConf()->isMajority(votes_acquired)) {
            ctx_->changeState(States::Leader);
        }
    }
    else {
        if(msg->term > state_->currentTerm_) {
            ctx_->logger_->warn("Received term correction from {} to {}", state_->currentTerm_, msg->term);
            // Update term if out of date
            state_->setTerm(msg->term);
            // Transition to follower
            ctx_->changeState(States::Follower);
        }
    }
};


leader_rpc::leader_rpc(raft_node *ctx, std::unique_ptr<raft_network> &&network, std::unique_ptr<state>&& state)
        : raft_rpc(ctx, std::move(network), std::move(state)) {
    ctx_->logger_->info("Transitioned to Leader");
    ctx_->logger_->info("New Term: {}", state_->currentTerm_);
    state_->resetTime(leader_idle_time, leader_idle_time + 1);
    state_->election_timer_.async_wait(std::bind(&leader_rpc::heartbeat, this, std::placeholders::_1));
}

void leader_rpc::heartbeat(const std::error_code &ec) {
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
    state_->election_timer_.async_wait(std::bind(&leader_rpc::heartbeat, this, std::placeholders::_1));
}

void leader_rpc::AppendEntries(std::shared_ptr<append_entries> msg) {
    ctx_->logger_->warn("Received heartbeat as leader!");
}

void leader_rpc::RequestVote(std::shared_ptr<request_votes> msg) {
    ctx_->logger_->warn("Received vote request as leader!");
}
