//
// Created by nathan on 08/06/15.
//

#include "raft_network.h"
#include "raft_node.h"

connection::connection(raft_node* node, std::shared_ptr<udp::socket> socket, udp::endpoint endpoint)
            : node_(node), socket_(socket), remote_endpoint_(endpoint) {

    do_receive();
}

void connection::do_receive() {
    socket_->async_receive_from(
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
    socket_->async_send_to(buf->data(), remote_endpoint_,
                  [buf](std::error_code /*ec*/, std::size_t /*bytes_sent*/) {});
}

raft_network::raft_network(raft_node* node, asio::io_service& io, const uuid_t &uuid,
                           const std::vector<raft_node_endpoint_t> &peers) {
    auto endpoint = *std::find_if(peers.begin(), peers.end(), [&uuid](auto &row){ return row.uuid == uuid; });
    auto socket = std::make_shared<udp::socket>(io, udp::endpoint(udp::v4(), endpoint.in_port));
    for(auto& peer : peers) {
        if(peer.uuid != uuid) {
            udp::resolver resolver(io);
            udp::resolver::query query(udp::v4(), peer.addr, std::to_string(peer.in_port));
            std::error_code ec;
            auto remote_endpoint = *resolver.resolve(query, ec);
            if(!ec) {
                connections_.emplace(peer.uuid, std::make_unique<connection>(node, socket, remote_endpoint));
            }
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
