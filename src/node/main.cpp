#include <iostream>
#include <array>
#include <asio/steady_timer.hpp>
#include <asio.hpp>
#include "Connection.h"
#include "NodeState/NodeState.h"

using namespace std;

int main(int argc, char* argv[]) {
    try {
        if(argc < 2 || argc > 3) {
            std::cerr << "Usage: node <port>\n";
            return 1;
        }

        State s("localhost", 12345, 12346);
        s.run();
    }
    catch(std::exception& e) {
        std::cerr << "Error" << std::endl;
        std::cerr << e.what() << std::endl;
    }


    return 0;
}