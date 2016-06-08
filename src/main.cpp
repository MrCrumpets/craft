#include <iostream>
#include <array>
#include <asio/steady_timer.hpp>
#include <asio.hpp>
#include <thread>
#include "node/raft_node.h"

using namespace std;

std::vector<raft_node_endpoint_t> peers{
        {1ul, "localhost", 12345, 13001},
        {2ul, "localhost", 12346, 13002},
        {3ul, "localhost", 12347, 13003}
};

int main(int argc, char* argv[]) {
    try {
        // Start raft_nodes
        std::vector<std::thread> workers;
        std::vector<raft_node> nodes;
        for (auto &peer : peers) {
            nodes.emplace_back(peer.uuid, std::make_shared<raft::config>(peers));
        }
        for (auto &node : nodes) {
            workers.push_back(std::thread([&node]() {
                node.run();
            }));
        }
        workers.push_back(std::thread([&nodes]() {
            while (true) {
                std::cout << "Assigned roles for cluster: " << std::endl;
                for (auto &node : nodes) {
                    std::cout << node.describe() << std::endl;
                }
                std::this_thread::sleep_for(2s);

            }
        }));

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
