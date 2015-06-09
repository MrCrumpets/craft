#include <iostream>
#include <array>
#include <asio/steady_timer.hpp>
#include <asio.hpp>
#include <chrono>

using namespace std;
using asio::ip::udp;

enum Message {
    AppendEntries,
    AppendEntriesResponse,
    RequestVote,
    RequestVoteResponse
};


class node {
public:
    node(asio::io_service& io_service, short listen_port, short send_port)
            : io_service_(io_service),
              socket_(io_service, udp::endpoint(udp::v4(), listen_port)),
              timer_(io_service)
    {
        std::srand(std::time(0));

        udp::resolver resolver(io_service);
        udp::resolver::query query(udp::v4(), "localhost", std::to_string(send_port));

        remote_endpoint_ = *resolver.resolve(query);
        out_buffer_[0] = AppendEntries;

        do_receive();
        do_send();
    }

private:
    void do_receive() {
        socket_.async_receive_from(
            asio::buffer(in_buffer_), remote_endpoint_,
            [this](std::error_code ec, std::size_t bytes_recvd) {
                if(!ec && bytes_recvd > 0) {
                    switch(in_buffer_[0]) {
                        case AppendEntries:
                            std::cout << "Append Entries Message\n";
                    }
                }

                do_receive();
            });
    }

    void do_send() {
        socket_.async_send_to(
            asio::buffer(out_buffer_), remote_endpoint_,
            [this](std::error_code /*ec*/, std::size_t /*bytes_sent*/){
                timer_.expires_from_now(std::chrono::milliseconds());
                timer_.wait();
                do_send();
            });
    }

    asio::steady_timer timer_;
    asio::io_service &io_service_;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    enum {
        election_timeout = 300,
    };
    std::array<Message, 1> out_buffer_;
    std::array<Message, 1> in_buffer_;
};

int main(int argc, char* argv[]) {
    try {
        if(argc < 2 || argc > 3) {
            std::cerr << "Usage: node <port>\n";
            return 1;
        }

        short send_port = -1;
        if(argc == 3) {
            send_port = std::atoi(argv[2]);
        }

        asio::io_service io_service;
        node c(io_service, std::atoi(argv[1]), send_port);
        io_service.run();

    }
    catch(std::exception& e) {
        std::cerr << "Error" << std::endl;
        std::cerr << e.what() << std::endl;
    }


    return 0;
}