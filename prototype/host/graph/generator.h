#pragma once
#include <stdint.h>
#include <vector>
#include "common.h"
struct edge_info_t {
    uint32_t to;
    bool async;
};

using network_adj_list_t = std::vector<std::vector<edge_info_t>>;
network_adj_list_t create_random_network();
std::vector<node_t> create_nodes();