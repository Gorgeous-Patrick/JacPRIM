#pragma once
#include "generator.h"
#include <bits/stdint-uintn.h>
#include <vector>
#include <stdint.h>
std::vector<uint32_t> generate_naive_node_assignment();
std::vector<uint32_t> generate_bfs_based_node_assignment(const network_adj_list_t & network);
std::vector<uint32_t> generate_bfs_based_async_node_assignment(const network_adj_list_t & network);
int ceil_div(int a, int b);