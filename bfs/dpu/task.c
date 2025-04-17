#include <mram.h>
#include <stdint.h>
#include <stdio.h>
#include "common.h"

__mram struct metadata_t metadata;
__mram struct node_t nodes[MAX_NODE_NUM_PER_DPU];
__mram struct walker_impl_t walker_impl[8];
__mram struct edge_impl_t edges[MAX_EDGE_NUM_PER_DPU];

int find_node(int node_id, struct metadata_t * metadata) {
    for (int i = 0; i < metadata->node_num; ++i) {
        if (metadata->node_available[i] == node_id) {
            return i;
        }
    }
    return -1;
}

void bfs_walker_func(struct walker_impl_t* this, struct node_t * here, struct metadata_t * metadata) {
    int node_id = here->id;
    for (int i = 0; i < metadata->edge_num; ++i) {
        if (edges[i].from == node_id) {
            int next_node_id = edges[i].to;
            if (this->walker.visited[next_node_id]) {
                continue;
            }
            // Push next node to the container
            this->container[this->container_size++] = next_node_id;
        }
    }
}

int main() {
    struct metadata_t metadata_local = metadata;
    struct node_t nodes_local[MAX_NODE_NUM_PER_DPU];
    struct walker_impl_t walker_impl_local = walker_impl[0];
    for (int i = 0; i < metadata_local.node_num; ++i) {
        nodes_local[i] = nodes[i];
    }
    printf("Node Num in this node: %ld\n", metadata.node_num);
    for (int i = 0; i < metadata.node_num; ++i) {
        printf("Node %d: %s\n", i, nodes_local[i].name);
    }

    if (walker_impl_local.container_size == 0) {
        printf("Container is empty\n");
        return 0;
    } else {
        printf("Container is not empty\n");
    }

    int run_on_node = walker_impl_local.container[0];
    int node_ind;
    while ((node_ind = find_node(run_on_node, &metadata_local)) != -1) {
        printf("Found\n");
        // Pop the node from the container
        for (int i = 1; i < walker_impl_local.container_size; ++i) {
            walker_impl_local.container[i - 1] = walker_impl_local.container[i];
        }
        walker_impl_local.container_size--;
        printf("Running on Node %d\n", run_on_node);
        bfs_walker_func(&walker_impl_local, &nodes_local[node_ind], &metadata_local);
        for (int i = 0; i < walker_impl_local.container_size; ++i) {
            printf("Container[%d]: %d\n", i, walker_impl_local.container[i]);
        }
        run_on_node = walker_impl_local.container[0];
    }
    walker_impl[0] = walker_impl_local;
    printf("Not found\n");
    return 0;
}