#include <vector>
#include "common.h"
#include <random>
#include <cstring>
#include "generator.h"


network_adj_list_t create_random_network() {
    // Create an adjacency list for a random network
    network_adj_list_t network(NODE_NUM);
    for (int i = 0; i < NODE_NUM; ++i) {
        for (uint32_t j = 0; j < NODE_NUM; ++j) {
            if (i != j && rand() % 2 == 0) {
                network[i].push_back({.to = j, .async = false});
            }
        }
    }
    return network;
}

std::vector<node_t> create_nodes() {
    // Create names for nodes
    std::vector<node_t> nodes(NODE_NUM);
    for (int i = 0; i < NODE_NUM; ++i) {
        std::string name;
        name = "Node_" + std::to_string(i);
        std::strncpy(nodes[i].name, name.c_str(), NAME_SIZE);
        nodes[i].id = i;
        nodes[i].name[NAME_SIZE - 1] = '\0'; // Ensure null-termination
    }
    return nodes;
}