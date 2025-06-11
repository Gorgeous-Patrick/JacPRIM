from data_prep import download_and_cache, get_subgraph
from partitioner import random_partition, metis_partition, calculate_performance, fennel_partition
import os
import faulthandler

SNAP_URL = "https://snap.stanford.edu/data/twitter-2010.txt.gz"
DATA_DIR = "snap_data"
RAW_FILE = os.path.join(DATA_DIR, "twitter.txt.gz")
EXTRACTED_FILE = os.path.join(DATA_DIR, "twitter.txt")
SUBGRAPH_FILE = os.path.join(DATA_DIR, "twitter-subgraph.txt")

def simulate_walkers(graph):
    walker_traces = dict()

    for node in graph.nodes():
        trace = [node]  # first visit self
        neighbors = list(graph.neighbors(node))
        trace.extend(neighbors)
        walker_traces[node] = trace

    return walker_traces

def main():
    download_and_cache(RAW_FILE, EXTRACTED_FILE, SNAP_URL, DATA_DIR)
    G = get_subgraph(EXTRACTED_FILE, SUBGRAPH_FILE, target_size=1000, limit=100000)
    walker_traces = simulate_walkers(G)
    sum = 0
    for i in range(10):
        partition = random_partition(G, num_partitions=5)
        perf = calculate_performance(walker_traces, partition)
        sum += perf
    print(f"Average edges crossing partition boundaries for random partitioning: {sum / 10}")
    perf = calculate_performance(walker_traces, metis_partition(G, num_partitions=5))
    # perf = calculate_performance(walker_traces, fennel_partition(G, num_partitions=5, capacity=200))
    print(f"Edges crossing partition boundaries for METIS partitioning: {perf}")
        



if __name__ == "__main__":
    faulthandler.enable()
    main()


# w1 with entry int {
#     min_idx, max_idx, min_value, max_value: int;
#     mid_idx = (min_idx + max_idx) // 2;
#     visit[-->]
#     visit[<--]
# }

# with entry {
#     node n1, n2, n3; (ints)
#     n1 --> n2;
#     n2 --> n3;

#     w = w1(2) spawn n2;

# }