#include <iostream>
#include <array>
#include <asio.hpp>

using namespace std;
using asio::ip::udp;


class server {
public:
    server(asio::io_service& io_service, short listen_port, short send_port)
            : io_service_(io_service),
              socket_(io_service, udp::endpoint(udp::v4(), listen_port))
    {
        if(send_port > 0) {
            udp::resolver resolver(io_service);
            udp::resolver::query query(udp::v4(), "localhost", std::to_string(send_port));
            udp::endpoint send_point = *resolver.resolve(query);

            const char *test = "test";
            socket_.async_send_to(asio::buffer(test, strlen(test)), send_point,
                    [this](std::error_code /*ec*/, std::size_t /*bytes_sent*/) {
                        std::cout << "Sent kickoff message." << std::endl;
                    });
        }
        do_receive();
    }

private:
    void do_receive() {
        socket_.async_receive_from(
            asio::buffer(data_, max_length), remote_endpoint_,
            [this](std::error_code ec, std::size_t bytes_recvd) {
                if(!ec && bytes_recvd > 0) {
                    std::cout.write(data_, bytes_recvd);
                    std::cout << endl;
                    do_send(bytes_recvd);
                }
                else {
                    do_receive();
                }
            });
    }

    void do_send(std::size_t length) {
        socket_.async_send_to(
            asio::buffer(data_, length), remote_endpoint_,
            [this](std::error_code /*ec*/, std::size_t /*bytes_sent*/){
                do_receive();
            });
    }

    asio::io_service &io_service_;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    enum { max_length = 1024 };
    char data_[max_length];
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
        server c(io_service, std::atoi(argv[1]), send_port);
        io_service.run();

    }
    catch(std::exception& e) {
        std::cerr << "Error" << std::endl;
        std::cerr << e.what() << std::endl;
    }


    return 0;
}