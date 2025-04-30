#include "common.h"
#include "generator.h"
#include "graph_cut.h"
#include <iostream>

std::vector<uint32_t> generate_naive_node_assignment() {
    // Assign nodes to DPUs
    std::vector<uint32_t> node_assignments(NODE_NUM);
    for (int i = 0; i < NODE_NUM; ++i) {
        node_assignments[i] = i % NUM_DPU;
    }
    return node_assignments;
}


int ceil_div(int a, int b) {
    return (a + b - 1) / b;
}

std::vector<int> bfs(const network_adj_list_t & network) {
    std::vector<int> visited(NODE_NUM, 0);
    std::vector<int> traversal_order;
    std::vector<int> queue;
    queue.push_back(0);
    visited[0] = 1;

    while (!queue.empty()) {
        int current_node = queue.front();
        queue.erase(queue.begin());
        traversal_order.push_back(current_node);

        for (const auto& neighbor : network[current_node]) {
            if (!visited[neighbor.to]) {
                visited[neighbor.to] = 1;
                queue.push_back(neighbor.to);
            }
        }
    }
    return traversal_order;
}

std::vector<std::vector<int>> get_next_steps(const network_adj_list_t & network, int current_node, std::vector<int> & visited) {
    std::vector<std::vector<int>> next_nodes;
    std::vector<int> one_level;
    for (const auto& neighbor : network[current_node]) {
        if (visited[neighbor.to]) {
            continue;
        }
        if (neighbor.async) {
            one_level.push_back(neighbor.to);
        } else {
            if (!one_level.empty()) {
                next_nodes.push_back(one_level);
                one_level.clear();
            }
            next_nodes.push_back({(int)neighbor.to});
        }
    }
    if (!one_level.empty()) {
        next_nodes.push_back(one_level);
    }
    return next_nodes;
}

std::vector<std::vector<int>> bfs_async(const network_adj_list_t & network, std::vector<int> & visited, int start) {
    std::vector<std::vector<int>> traversal_order;
    std::vector<std::vector<int>> queue;
    queue.push_back({start});
    visited[start] = 1;

    while (!queue.empty()) {
        std::vector<int> current_nodes = queue.front();
        queue.erase(queue.begin());
        traversal_order.push_back(current_nodes);

        std::vector<std::vector<std::vector<int>>> next_nodes;
        int max_length = 0;
        for (const auto & current_node : current_nodes) {
            auto tmp = get_next_steps(network, current_node, visited);
            max_length = std::max(max_length, (int)tmp.size());
            next_nodes.push_back(tmp);
        }

        // Merge the next nodes layer by layer
        // One level is the concatenation of all next nodes of the same index
        for (int i = 0; i < max_length; ++i) {
            std::vector<int> one_level;
            for (const auto & next_node : next_nodes) {
                if (i < next_node.size()) {
                    one_level.insert(one_level.end(), next_node[i].begin(), next_node[i].end());
                }
            }

            if (!one_level.empty()) {
                for (const auto & node : one_level) {
                    if (!visited[node]) {
                        visited[node] = 1;
                    }
                }
                queue.push_back(one_level);
            }
        }
    }
    return traversal_order;
}


std::vector<uint32_t> generate_bfs_based_node_assignment(const network_adj_list_t & network) {
  std::vector<int> traversal_order = bfs(network);
  std::vector<uint32_t> node_assignments(NODE_NUM);
  for (size_t i = 0; i < traversal_order.size(); ++i) {
    node_assignments[traversal_order[i]] = i / (ceil_div(NODE_NUM, NUM_DPU));
  }
  return node_assignments;
}


std::vector<uint32_t> generate_bfs_based_async_node_assignment(const network_adj_list_t & network) {
    std::vector<int> visited(network.size(), 0);
    std::vector<std::vector<int>> traversal_order = bfs_async(network, visited, 0);

    // Print the traversal order
    for (const auto& layer : traversal_order) {
        std::cout << "Layer: ";
        for (int node : layer) {
            std::cout << node << " ";
        }
        std::cout << std::endl;
    }

    // Initialize node assignments
    std::vector<uint32_t> node_assignments(network.size(), -1);
    std::vector<int> dpu_node_count(network.size(), 0); // Tracks the number of nodes assigned to each DPU

    // Assign nodes layer by layer
    for (const auto& layer : traversal_order) {
        int dpu_index = 0;

        for (int node : layer) {
            // Find the next available DPU
            for (int i = 0; i < NUM_DPU; ++i) {
                if (dpu_node_count[dpu_index] < MAX_NODE_NUM_PER_DPU) {
                    break;
                }
                dpu_index = (dpu_index + 1) % NUM_DPU;
            }
            // If all DPUs are full, break out of the loop
            if (dpu_node_count[dpu_index] >= MAX_NODE_NUM_PER_DPU) {
                std::cerr << "All DPUs are full. Cannot assign more nodes." << std::endl;
                throw std::runtime_error("Node assignment failed");
            }
            // Assign the node to the current DPU
            node_assignments[node] = dpu_index;
            dpu_node_count[dpu_index]++;

            // Move to the next DPU for the next node
            dpu_index = (dpu_index + 1) % NUM_DPU;
        }
    }

    return node_assignments;
}