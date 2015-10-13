#include <iostream>
#include <array>
#include <asio/steady_timer.hpp>
#include <asio.hpp>
#include <thread>
#include "node/raft_node.h"

using namespace std;

std::vector<raft_node_endpoint_t> peers{
        {1ul, "localhost", 12345, 13001},
        {2ul, "localhost", 12346, 13002}
};

int main(int argc, char* argv[]) {
    try {
        // Start raft_nodes
        std::vector<std::thread> workers;
        std::for_each(peers.begin(), peers.end(), [&workers](auto &config){
            workers.push_back(std::thread([&config](){
                raft_node s(config.uuid, std::make_shared<raft::config>(peers));
                s.run();
            }));
        });

        // Wait for raft_nodes to finish
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