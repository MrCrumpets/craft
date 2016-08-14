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
#include <spdlog/spdlog.h>
#include "raft_network.h"
#include "state.h"

//class raft_node;

enum class State {
    Follower, Candidate, Leader
};

enum Constants {
    election_timeout = 1000,
    leader_idle_time = 250
};

class raft_rpc {
private:
    raft_node *ctx_;
    std::unique_ptr<state> state_;
    std::unique_ptr<raft_network> network_;
    State mode_;

    void reset_election_timer();

    void changeState(State s);

    void heartbeat(const std::error_code &e = std::error_code::error_code());

public:
    void rpc(std::shared_ptr<raft_message> msg);

    void AppendEntries(std::shared_ptr<append_entries>);

    void RequestVote(std::shared_ptr<request_votes>);

    void voteResponse(std::shared_ptr<response>);

    raft_rpc(raft_node *context, std::unique_ptr<raft_network> &&network, std::unique_ptr<state> &&state);


};

class raft_node {
    node_id_t uuid_;
    asio::io_service io_service_;
    raft_rpc rpc_;
public:
    std::shared_ptr<spdlog::logger> logger_;

    raft_node(const raft_node &) = delete;

    raft_node &operator=(const raft_node &) = delete;

    raft_node(const node_id_t &uuid, std::shared_ptr<raft::config> conf);

    void dispatch_message(std::shared_ptr<raft_message> m);

    void run();
};

#endif //CRAFT_NODESTATE_H
