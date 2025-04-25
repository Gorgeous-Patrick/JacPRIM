#include <bits/stdint-uintn.h>
#include <vector>
#include "common.h"
#include <random>
#include <cstring>
#include "generator.h"


network_adj_list_t create_random_network(uint32_t size) {
    // Create an adjacency list for a random network
    network_adj_list_t network(size);
    for (int i = 0; i < size; ++i) {
        for (uint32_t j = 0; j < NODE_NUM; ++j) {
            if (i != j && rand() % 2 == 0) {
                network[i].push_back({.to = j, .async = false});
            }
        }
    }
    return network;
}

network_adj_list_t create_va_network(uint32_t vector_size) {
    network_adj_list_t network(1 + 2 * vector_size);
    for (uint32_t i = 0; i < vector_size; ++i) {
        network[0].push_back({.to = 1 + i, .async = true});
        network[1 + i].push_back({.to = 1 + vector_size + i, .async = false});
    }
    return network;
}

std::vector<node_t> create_nodes(uint64_t size) {
    // Create names for nodes
    std::vector<node_t> nodes(size);
    for (int i = 0; i < size; ++i) {
        std::string name;
        name = "Node_" + std::to_string(i);
        std::strncpy(nodes[i].name, name.c_str(), NAME_SIZE);
        nodes[i].id = i;
        nodes[i].name[NAME_SIZE - 1] = '\0'; // Ensure null-termination
    }
    return nodes;
}