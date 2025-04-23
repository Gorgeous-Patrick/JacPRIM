#include "common.h"
#include "graph_cut.h"

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


std::vector<uint32_t> generate_bfs_based_node_assignment(const network_adj_list_t & network) {
  std::vector<int> traversal_order = bfs(network);
  std::vector<uint32_t> node_assignments(NODE_NUM);
  for (size_t i = 0; i < traversal_order.size(); ++i) {
    node_assignments[traversal_order[i]] = i / (ceil_div(NODE_NUM, NUM_DPU));
  }
  return node_assignments;
}