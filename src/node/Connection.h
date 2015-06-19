//
// Created by nathan on 08/06/15.
//

#ifndef CRAFT_CONNECTION_H
#define CRAFT_CONNECTION_H


#include <asio/io_service.hpp>
#include <asio/buffer.hpp>
#include <chrono>
#include <asio/ip/udp.hpp>
#include <asio/steady_timer.hpp>

using asio::ip::udp;

namespace Network {

    enum class MessageType {
        AppendEntries,
        AppendEntriesResponse,
        RequestVote,
        RequestVoteResponse
    };

    class Message {
        MessageType type_;
    };

    class Connection {
    public:
        Connection(asio::io_service &io_service, short listen_port, short send_port);

    private:
        void do_receive();
        void do_send(Message &);

        asio::steady_timer timer_;
        asio::io_service &io_service_;
        udp::socket socket_;
        udp::endpoint remote_endpoint_;
        enum {
            election_timeout = 300,
        };
        std::array<MessageType, 1> out_buffer_;
        std::array<MessageType, 1> in_buffer_;

    };
}



#endif //CRAFT_CONNECTION_H
