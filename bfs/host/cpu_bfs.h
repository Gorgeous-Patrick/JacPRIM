#pragma once
#include <vector>
#include <stdint.h>
std::vector<uint32_t> generate_bfs_based_node_assignment(const std::vector<std::vector<long>>& network);
int ceil_div(int a, int b);