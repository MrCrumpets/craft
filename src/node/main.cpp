#include <iostream>
#include <array>
#include <asio.hpp>

using namespace std;
using asio::ip::udp;


class server {
public:
    server(asio::io_service& io_service, short port)
            : io_service_(io_service),
              socket_(io_service, udp::endpoint(udp::v4(), port))
    {
        do_receive();
    }

private:
    void do_receive() {
        socket_.async_receive_from(
            asio::buffer(data_, max_length), sender_endpoint_,
            [this](std::error_code ec, std::size_t bytes_recvd) {
                if(!ec && bytes_recvd > 0) {
                    do_send(bytes_recvd);
                }
                else {
                    do_receive();
                }
            });
    }

    void do_send(std::size_t length) {
        socket_.async_send_to(
            asio::buffer(data_, length), sender_endpoint_,
            [this](std::error_code /*ec*/, std::size_t /*bytes_sent*/){
                do_receive();
            });
    }

    asio::io_service &io_service_;
    udp::socket socket_;
    udp::endpoint sender_endpoint_;
    enum { max_length = 1024 };
    char data_[max_length];
};

int main(int argc, char* argv[]) {
    try {
        if(argc != 2) {
            std::cerr < "Usage: node <port>\n";
            return 1;
        }

        asio::io_service io_service;
        server c(io_service, std::atoi(argv[1]));
        io_service.run();

    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }


    return 0;
}