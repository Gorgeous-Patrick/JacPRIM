from collections import defaultdict, deque
import random
from collections import Counter
import networkx as nx
import metis

def fennel_partition(graph, num_partitions, capacity):
    N = graph.number_of_nodes()
    beta = 1.5
    alpha = N / (num_partitions ** beta)
    lambd = 1.0

    partitions = defaultdict(set)
    partition_sizes = [0] * num_partitions
    assignment = {}

    nodes = list(graph.nodes())
    random.shuffle(nodes)  # Random streaming order

    for node in nodes:
        neighbor_counts = [0] * num_partitions
        for neighbor in graph.neighbors(node):
            if neighbor in assignment:
                p = assignment[neighbor]
                neighbor_counts[p] += 1

        # Compute FENNEL score for each partition
        scores = []
        for p in range(num_partitions):
            if partition_sizes[p] >= capacity:
                scores.append(float('inf'))  # prevent overflow
            else:
                score = -lambd * neighbor_counts[p] + alpha * (partition_sizes[p] ** beta)
                scores.append(score)

        # Select partition with minimal score
        best_partition = scores.index(min(scores))
        assignment[node] = best_partition
        partitions[best_partition].add(node)
        partition_sizes[best_partition] += 1

    return assignment, partitions

def metis_partition(graph, num_partitions):
    graph = graph.copy()
    bfs_visit_orders = [list(nx.bfs_tree(graph, source=node).nodes()) for node in graph.nodes()]
    graph.clear_edges()
    edge_counter = Counter()
    for order in bfs_visit_orders:
        edges = [(order[i], order[i+1]) for i in range(len(order)-1)]
        edge_counter.update(edges)
    
    # Rebuild the subgraph with BFS edges
    for (u, v), weight in edge_counter.items():
        graph.add_edge(u, v, weight=weight)

    (edgecuts, parts) = metis.part_graph(metis.networkx_to_metis(graph), nparts=num_partitions, recursive=True, tpwgts=[1/num_partitions]*num_partitions, ufactor=30)
    res = dict()
    for i, name in enumerate(graph.nodes()):
        res[name] = parts[i]
    return res


def random_partition(graph, num_partitions):
    graph = graph.copy()
    res = dict()
    for i, name in enumerate(graph.nodes()):
        res[name] = random.randint(0, num_partitions - 1)
    return res

def calculate_performance(walker_traces: dict, label: dict[str, int]):
    res = 0
    for node, trace in walker_traces.items():
        edges = list(zip(trace[:-1], trace[1:]))
        for u, v in edges:
            if label[u] != label[v]:
                # print(f"Edge ({u}, {v}) crosses partition boundaries: {label[u]} vs {label[v]}")
                res += 1
    print(f"Total edges crossing partition boundaries: {res}")
    return res

