#include <iostream>
#include <array>
#include <asio.hpp>

using namespace std;
using asio::ip::tcp;


int main(int argc, char* argv[]) {
    try {
        if(argc != 2) {
            std::cerr << "Usage: client <host>" << std::endl;
            return 1;
        }
        asio::io_service io_service;
        tcp::resolver resolver(io_service);
        tcp::resolver::query query(argv[1], "daytime");
        auto endpoint_iterator = resolver.resolve(query);
        tcp::socket socket(io_service);
        connect(socket, endpoint_iterator);
        for(;;) {
            std::array<char, 128> buf;
            asio::error_code error;
            size_t len = socket.read_some(asio::buffer(buf), error);
            if(error == asio::error::eof)
                break;
            else if(error)
                throw asio::system_error(error);

            std::cout.write(buf.data(), len);
        }
    }
    catch(std::exception& e) {
        std::cerr << e.what() << std::endl;
    }


    return 0;
}