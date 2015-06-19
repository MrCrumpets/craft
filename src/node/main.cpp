#include <iostream>
#include <array>
#include <asio/steady_timer.hpp>
#include <asio.hpp>
#include <thread>
#include "NodeState/NodeState.h"

using namespace std;

std::vector<std::pair<short, std::string>> nodes {
        {12345, "localhost"},
        {12346, "localhost"},
        {12347, "localhost"},
        {12348, "localhost"},
        {12349, "localhost"},
};

int main(int argc, char* argv[]) {
    try {
        if(argc < 2 || argc > 3) {
            std::cerr << "Usage: node <port>\n";
            return 1;
        }
        std::vector<std::thread> workers;
        std::for_each(nodes.begin(), nodes.end(), [&workers](std::pair<short, std::string> node){
            workers.push_back(std::thread([node](){
                State s(node.second, node.first, nodes);
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