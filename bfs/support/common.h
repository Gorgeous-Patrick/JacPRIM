#pragma once

#define NODE_NUM 100
#define NAME_SIZE 30
#define MAX_NODE_NUM_PER_DPU 8
#define NUM_DPU 20
#define MAX_OUTPUT 1000
#define MAX_EDGE_NUM_PER_DPU 1000

struct node_t {
  char name[NAME_SIZE];
  int id;
};

struct walker_t {
  char visited[NODE_NUM];
};

struct walker_impl_t {
  int container[NODE_NUM];
  int container_size;
  char output[MAX_OUTPUT + 1];
  struct walker_t walker;
};

struct edge_impl_t {
  int from;
  int to;
};

struct metadata_t {
  long node_num;
  int node_available[MAX_NODE_NUM_PER_DPU];
  int edge_num;
};