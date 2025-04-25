#include <bits/stdint-uintn.h>
#include <cstdint>
#include <dpu>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
#include "common.h"
#include <cstring>
#include "graph/generator.h"
#include "graph/graph_cut.h"

// #define BFS_BASED_ASSIGNMENT

using namespace dpu;

std::vector<metadata_t> send_metadata_to_dpu(const std::vector<DpuSet *> &dpus, const std::vector<uint32_t> &node_assignments, network_adj_list_t &network) {
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

void send_nodes_to_dpu(const std::vector<DpuSet *> &dpus, const std::vector<node_t> &nodes, const std::vector<metadata_t> &metadata, const network_adj_list_t &network) {
    for (size_t i = 0; i < dpus.size(); ++i) {
        std::vector<node_t> nodes_to_send(align_data_size(metadata[i].node_num));
        std::vector<edge_impl_t> edges_to_send;
        for (size_t j = 0; j < metadata[i].node_num; ++j) {
            nodes_to_send[j] = nodes[metadata[i].node_available[j]];
            for (const auto &neighbor : network[metadata[i].node_available[j]]) {
                edge_impl_t edge;
                edge.from = metadata[i].node_available[j];
                edge.to = neighbor.to;
                edges_to_send.push_back(edge);
            }
        }
        auto edges_to_send_aligned = align_data_size(edges_to_send);
        dpus[i]->copy("nodes", std::vector<std::vector<node_t>>(1, nodes_to_send));
        dpus[i]->copy("edges", std::vector<std::vector<edge_impl_t>>(1, edges_to_send_aligned));
    }
}

int main(int argc, char **argv) {
    auto network = create_va_network(1000);
    auto nodes = create_nodes(network.size());
    auto node_assignments_async = generate_bfs_based_async_node_assignment(network);

    std::cout << "Generated the graph" << std::endl;
    // Print out the node assignments
    std::cout << "Node assignments:" << std::endl;
    for (size_t i = 0; i < node_assignments_async.size(); ++i) {
        std::cout << "Node " << i << " assigned to DPU " << node_assignments_async[i] << std::endl;
    }

    // walker_impl_t walker = create_walker();
    // int cnt = 0;
    // while (walker.container_size > 0) {
    //     auto system = DpuSet::allocate(NUM_DPU);
    //     auto dpus = system.dpus();
    //     for (auto dpu : dpus) {
    //         dpu->load("dpu");
    //     }
    //     auto metadata = send_metadata_to_dpu(system.dpus(), node_assignments, network);
    //     std::cout << "Sent meta data to DPU" << std::endl;
    //     send_nodes_to_dpu(system.dpus(), nodes, metadata, network);
    //     std::cout << "Send edge to DPU" << std::endl;

    //     std::cout << "====== New Iteration ======" << std::endl;
    //     uint32_t dpu_id = distribute_walker(walker, dpus, node_assignments);
    //     try {
    //         dpus[dpu_id]->exec();
    //     } catch (...) {}
    //     for (uint32_t i = 0; i < dpus.size(); i++) {
    //         auto dpu = dpus[i];
    //         dpu->log(std::cout);
    //     }
    //     walker = collect_walker(dpus, dpu_id, node_assignments);
    //     cnt++;
    // }

    // std::cout << "Walker finished after " << cnt << " iterations" << std::endl;
    // return 0;
}