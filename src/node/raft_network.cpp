//
// Created by nathan on 08/06/15.
//

#include "raft_network.h"
#include "raft_node.h"

connection::connection(raft_node* node, asio::io_service &io_service, const std::string &addr, const short listen_port, const short send_port)
            : node_(node), socket_(io_service, udp::endpoint(udp::v4(), listen_port)), timer_(io_service) {

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), addr, std::to_string(send_port));
    remote_endpoint_ = *resolver.resolve(query);

    do_receive();
}

void connection::do_receive() {
    socket_.async_receive_from(
            asio::buffer(in_buffer_, in_buffer_.size()), remote_endpoint_,
            [this](const std::error_code &ec, std::size_t bytes_recvd) {
                if(!ec) {
                    std::shared_ptr<raft_message> m;
                    {
                        std::istringstream iss(std::string(in_buffer_.begin(), in_buffer_.begin() + bytes_recvd));
                        cereal::JSONInputArchive iarchive(iss);
                        iarchive(m);
                    }
                    node_->dispatch_message(m);
                    do_receive();
                }
            });
}

auto serialize_msg(std::shared_ptr<raft_message> msg) {
    auto buf = std::make_shared<asio::streambuf>();
    std::ostream msg_stream(&(*buf));
    {
        cereal::JSONOutputArchive oarchive(msg_stream);
        oarchive(msg);
    }
    return buf;
}

void connection::do_send(std::shared_ptr<raft_message> m) {
    auto buf = serialize_msg(m);
    socket_.async_send_to(buf->data(), remote_endpoint_,
                  [buf](std::error_code /*ec*/, std::size_t /*bytes_sent*/) {});
}

raft_network::raft_network(raft_node* node, asio::io_service& io, const uuid_t &uuid,
                           const std::vector<raft_node_endpoint_t> &peers) {
    auto endpoint = *std::find_if(peers.begin(), peers.end(), [&uuid](auto &row){ return row.uuid == uuid; });
    for(auto& peer : peers) {
        if(peer.uuid != uuid) {
            connections_.emplace(peer.uuid, std::make_unique<connection>(node, io, peer.addr, endpoint.in_port, peer.in_port));
        }
    }
}

void raft_network::broadcast(std::shared_ptr<raft_message> message) {
    for(auto &connection : connections_) {
        connection.second->do_send(message);
    }
}

void raft_network::send_to_id(uuid_t id, std::shared_ptr<raft_message> message) {
    if(connections_.count(id)) {
        connections_[id]->do_send(message);
    }
    else {
        std::cerr << "id not found" << std::endl;
    }
}
