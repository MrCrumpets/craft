//
// Created by nathan on 08/06/15.
//

#ifndef CRAFT_NODESTATE_H
#define CRAFT_NODESTATE_H

#include <stdint.h>
#include <vector>
#include <memory>
#include <atomic>
#include <asio/deadline_timer.hpp>
#include <asio/basic_waitable_timer.hpp>
#include <asio/steady_timer.hpp>
#include "raft_network.h"
#include "state.h"

//class raft_node;

enum class States {
    Follower, Candidate, Leader
};

enum Constants {
    election_timeout = 500,
    leader_idle_time = 250
};

class raft_rpc {
protected:
    raft_node *ctx_;
    std::unique_ptr<state> state_;
    std::unique_ptr<raft_network> network_;

public:
    virtual void AppendEntries(std::shared_ptr<append_entries>) = 0;

    virtual void RequestVote(std::shared_ptr<request_votes>) = 0;

    virtual void voteResponse(std::shared_ptr<response>) {};

    virtual std::string getStateName() = 0;

    raft_rpc(raft_node *context, std::unique_ptr<raft_network> &&network, std::unique_ptr<state> &&state) : ctx_(context), network_(std::move(network)), state_(std::move(state)) {}

    std::unique_ptr<state> getState() {
       return std::move(state_);
    }

    std::unique_ptr<raft_network> getNetwork() {
        return std::move(network_);
    }

};

class candidate_rpc : public raft_rpc {
    std::atomic<int> votes_acquired;

public:
    candidate_rpc(raft_node *ctx, std::unique_ptr<raft_network> &&network, std::unique_ptr<state> &&state);

    void AppendEntries(std::shared_ptr<append_entries> msg);

    void RequestVote(std::shared_ptr<request_votes> msg);

    void voteResponse(std::shared_ptr<response>);

    std::string getStateName() { return "candidate"; };
};

class follower_rpc : public raft_rpc {


public:
    follower_rpc(raft_node *ctx, std::unique_ptr<raft_network> &&network, std::unique_ptr<state> &&state);

    void AppendEntries(std::shared_ptr<append_entries> msg);

    void RequestVote(std::shared_ptr<request_votes> msg);

    std::string getStateName() { return "follower"; };
};

class leader_rpc : public raft_rpc {

public:
    leader_rpc(raft_node *ctx, std::unique_ptr<raft_network> &&network, std::unique_ptr<state> &&state);

    void AppendEntries(std::shared_ptr<append_entries> msg);

    void RequestVote(std::shared_ptr<request_votes> msg);

    void heartbeat(const std::error_code &ec);

    std::string getStateName() { return "leader"; };
};

class raft_node {
    // Misc
    node_id_t uuid_;
    asio::io_service io_service_;
    std::unique_ptr<raft_rpc> mode_;
public:
    raft_node(const raft_node &) = delete;

    raft_node &operator=(const raft_node &) = delete;

    raft_node(const node_id_t &uuid, std::shared_ptr<raft::config> conf);

    raft_node(raft_node &&) noexcept;

    void changeState(States s);

    std::string describe() {
        return std::to_string(uuid_) + " " + mode_->getStateName();
    }

    void dispatch_message(std::shared_ptr<raft_message> m) const;

    void run();
};

#endif //CRAFT_NODESTATE_H
