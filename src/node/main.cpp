#include <iostream>
#include <array>
#include <asio/steady_timer.hpp>
#include <asio.hpp>
#include <thread>
#include "NodeState/NodeState.h"

using namespace std;

std::vector<short> ports {12345, 12346, 12347, 12348, 12349};

int main(int argc, char* argv[]) {
    try {
        if(argc < 2 || argc > 3) {
            std::cerr << "Usage: node <port>\n";
            return 1;
        }
        std::vector<std::thread> workers;
        std::for_each(ports.begin(), ports.end(), [&workers](short port){
            workers.push_back(std::thread([port](){
                State s("localhost", port, ports);
                s.run();
            }));
        });
        std::for_each(workers.begin(), workers.end(), [](std::thread &t){
            t.join();
        });
    }
    catch(std::exception& e) {
        std::cerr << "Error" << std::endl;
        std::cerr << e.what() << std::endl;
    }


    return 0;
}