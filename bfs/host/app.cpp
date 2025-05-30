#include <bits/stdint-uintn.h>
#include <cstdint>
#include <dpu>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "common.h"
#include <cstring>
#include "cpu_bfs.h"

// #define BFS_BASED_ASSIGNMENT

using namespace dpu;

std::vector<std::vector<long>> create_random_network() {
    // Create an adjacency list for a random network
    std::vector<std::vector<long>> network(NODE_NUM);
    for (int i = 0; i < NODE_NUM; ++i) {
        for (int j = 0; j < NODE_NUM; ++j) {
            if (i != j && rand() % 2 == 0) {
                network[i].push_back(j);
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

std::vector<uint32_t> generate_naive_node_assignment() {
    // Assign nodes to DPUs
    std::vector<uint32_t> node_assignments(NODE_NUM);
    for (int i = 0; i < NODE_NUM; ++i) {
        node_assignments[i] = i % NUM_DPU;
    }
    return node_assignments;
}

std::vector<metadata_t> send_metadata_to_dpu(const std::vector<DpuSet *> &dpus, const std::vector<uint32_t> &node_assignments, const std::vector<std::vector<long>> &network) {
    std::vector<metadata_t> metadata(dpus.size());
    for (size_t i = 0; i < dpus.size(); ++i) {
        metadata[i].node_num = 0;
        for (size_t j = 0; j < NODE_NUM; ++j) {
            if (node_assignments[j] == i) {
                metadata[i].node_available[metadata[i].node_num++] = j;
                metadata[i].edge_num += network[j].size();
            }
        }
        dpus[i]->copy("metadata", std::vector<metadata_t>(1, metadata[i]));
    }
    return metadata;
}

walker_impl_t create_walker() {
    walker_impl_t walker;
    walker.container_size = 1;
    for (int i = 0; i < NODE_NUM; ++i) {
        walker.container[i] = -1;
    }
    walker.container[0] = 0;
    return walker;
}

uint32_t distribute_walker(const walker_impl_t & walker, const std::vector<DpuSet *> &dpus, const std::vector<uint32_t> &node_assignments) {
    int next_node_id = walker.container[0];
    std::cout << "Next node id: " << next_node_id << std::endl;
    for (auto & dpu : dpus) {
        dpu->copy("walker_impl", std::vector<walker_impl_t>(8, walker));
    }
    return node_assignments[next_node_id];
}

walker_impl_t collect_walker(const std::vector<DpuSet *> &dpus, uint32_t dpu_id, const std::vector<uint32_t> &node_assignments) {
    // Collect the walker from one DPU.
    std::vector<std::vector<walker_impl_t>> walkers(dpus.size(), std::vector<walker_impl_t>(8));

    dpus[dpu_id]->copy(walkers, "walker_impl");
    auto walker = walkers[0][0];
    std::cout << "Walker container size: " << walker.container_size << std::endl;
    for (int i = 0; i < walker.container_size; ++i) {
        std::cout << "Container[" << i << "]: " << walker.container[i] << std::endl;
    }
    std::cout << "Walker output: " << walker.output << std::endl;
    return walker;
}

int align_data_size(int size) {
    return (ceil_div(size, 8)) * 8;
}
template <typename T>
std::vector<T> align_data_size(std::vector<T> vec) {
    int size = align_data_size(vec.size());
    vec.resize(size);
    return vec;
}

void send_nodes_to_dpu(const std::vector<DpuSet *> &dpus, const std::vector<node_t> &nodes, const std::vector<metadata_t> &metadata, const std::vector<std::vector<long>> &network) {
    for (size_t i = 0; i < dpus.size(); ++i) {
        std::vector<node_t> nodes_to_send(align_data_size(metadata[i].node_num));
        std::vector<edge_impl_t> edges_to_send;
        for (size_t j = 0; j < metadata[i].node_num; ++j) {
            nodes_to_send[j] = nodes[metadata[i].node_available[j]];
            for (const auto &neighbor : network[metadata[i].node_available[j]]) {
                edge_impl_t edge;
                edge.from = metadata[i].node_available[j];
                edge.to = neighbor;
                edges_to_send.push_back(edge);
            }
        }
        auto edges_to_send_aligned = align_data_size(edges_to_send);
        dpus[i]->copy("nodes", std::vector<std::vector<node_t>>(1, nodes_to_send));
        dpus[i]->copy("edges", std::vector<std::vector<edge_impl_t>>(1, edges_to_send_aligned));
    }
}

int run() {
    const auto network = create_random_network();
    const auto nodes = create_nodes();
    #ifdef BFS_BASED_ASSIGNMENT
    auto node_assignments = generate_bfs_based_node_assignment(network);
    #else
    auto node_assignments = generate_naive_node_assignment();
    #endif

    std::cout << "Generated the graph" << std::endl;
    walker_impl_t walker = create_walker();
    int cnt = 0;
    while (walker.container_size > 0) {
        auto system = DpuSet::allocate(NUM_DPU);
        auto dpus = system.dpus();
        for (auto dpu : dpus) {
            dpu->load("dev");
        }
        auto metadata = send_metadata_to_dpu(system.dpus(), node_assignments, network);
        std::cout << "Sent meta data to DPU" << std::endl;
        send_nodes_to_dpu(system.dpus(), nodes, metadata, network);
        std::cout << "Send edge to DPU" << std::endl;

        std::cout << "====== New Iteration ======" << std::endl;
        uint32_t dpu_id = distribute_walker(walker, dpus, node_assignments);
        try {
            dpus[dpu_id]->exec();
        } catch (...) {}
        for (uint32_t i = 0; i < dpus.size(); i++) {
            auto dpu = dpus[i];
            dpu->log(std::cout);
        }
        walker = collect_walker(dpus, dpu_id, node_assignments);
        cnt++;
    }

    // std::cout << "Walker finished after " << cnt << " iterations" << std::endl;
    return cnt;
}

int main() {
    int sum = 0;
    for (int i = 0; i < 10; ++i) {
        std::cout << "Running iteration " << i + 1 << std::endl;
        int cnt = run();
        sum += cnt;
        std::cout << "Walker finished after " << cnt << " iterations" << std::endl;
    }
    std::cout << "Average iterations: " << static_cast<double>(sum) / 10 << std::endl;
}