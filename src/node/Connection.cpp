//
// Created by nathan on 08/06/15.
//

#include "Connection.h"

using Network::Connection;


Connection::Connection(asio::io_service &io_service, short listen_port, short send_port)
            : io_service_(io_service),
            socket_(io_service, udp::endpoint(udp::v4(), listen_port)),
            timer_(io_service) {

    udp::resolver resolver(io_service);
    udp::resolver::query query(udp::v4(), "localhost", std::to_string(send_port));

    remote_endpoint_ = *resolver.resolve(query);
    out_buffer_[0] = MessageType::AppendEntries;

    do_receive();
}

void Connection::do_receive() {
    socket_.async_receive_from(
            asio::buffer(in_buffer_), remote_endpoint_,
            [this](std::error_code ec, std::size_t bytes_recvd) {
                if (!ec && bytes_recvd > 0) {
                    switch (in_buffer_[0]) {
                        case MessageType::AppendEntries:
                            std::cout << "Append Entries MessageType\n";
                            break;
                    }
                }

                do_receive();
            });
}

void Connection::do_send(std::string &host, short port, Message &m) {
    socket_.async_send_to(
            asio::buffer(out_buffer_), remote_endpoint_,
            [this](std::error_code /*ec*/, std::size_t /*bytes_sent*/) {
            });
}
