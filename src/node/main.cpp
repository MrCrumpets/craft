#include <iostream>
#include <array>
#include <asio/steady_timer.hpp>
#include <asio.hpp>
#include "Connection.h"

using namespace std;


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
        Connection c(io_service, std::atoi(argv[1]), send_port);
        io_service.run();

    }
    catch(std::exception& e) {
        std::cerr << "Error" << std::endl;
        std::cerr << e.what() << std::endl;
    }


    return 0;
}