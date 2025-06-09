import os
import urllib.request
import gzip
import shutil
import networkx as nx
import random
from tqdm import tqdm
from networkx.drawing.nx_agraph import write_dot
import pymetis
import matplotlib.pyplot as plt
import metis
from collections import Counter


SNAP_URL = "https://snap.stanford.edu/data/wiki-topcats.txt.gz"
DATA_DIR = "snap_data"
RAW_FILE = os.path.join(DATA_DIR, "wiki-topcats.txt.gz")
EXTRACTED_FILE = os.path.join(DATA_DIR, "wiki-topcats.txt")
SUBGRAPH_FILE = os.path.join(DATA_DIR, "wiki-topcats-subgraph.txt")

def download_and_cache():
    os.makedirs(DATA_DIR, exist_ok=True)
    if not os.path.exists(RAW_FILE):
        print("Downloading SNAP dataset...")
        urllib.request.urlretrieve(SNAP_URL, RAW_FILE)
    else:
        print("Dataset already downloaded.")

    if not os.path.exists(EXTRACTED_FILE):
        print("Extracting...")
        with gzip.open(RAW_FILE, 'rb') as f_in:
            with open(EXTRACTED_FILE, 'wb') as f_out:
                shutil.copyfileobj(f_in, f_out)
    else:
        print("Dataset already extracted.")

def load_full_graph():
    if os.path.exists(SUBGRAPH_FILE):
        return nx.read_edgelist(SUBGRAPH_FILE, create_using=nx.DiGraph(), data=False)
    print("Loading full graph...")
    G = nx.DiGraph()
    with open(EXTRACTED_FILE, 'r') as f:
        for line in tqdm(f):
            u, v = map(int, line.strip().split())
            G.add_edge(u, v)
    print(f"Full graph loaded: {G.number_of_nodes()} nodes, {G.number_of_edges()} edges")
    return G

import networkx as nx
import random
from collections import defaultdict, deque

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


def extract_subgraph(G, target_size=1000):
    print(f"Extracting subgraph with ~{target_size} nodes...")
    # Cache the results to avoid recomputing
    if os.path.exists(SUBGRAPH_FILE):
        print(f"Subgraph already exists at {SUBGRAPH_FILE}. Loading...")
        return nx.read_edgelist(SUBGRAPH_FILE, create_using=nx.DiGraph(), data=False)
    else:
        nodes = list(G.nodes())
        random.seed(42)
        seed_node = random.choice(nodes)
        sub_nodes = set()

        # BFS expansion until target size
        queue = [seed_node]
        while queue and len(sub_nodes) < target_size:
            node = queue.pop(0)
            if node in sub_nodes:
                continue
            sub_nodes.add(node)
            neighbors = list(G.successors(node)) + list(G.predecessors(node))
            random.shuffle(neighbors)
            queue.extend(neighbors)

        subgraph = G.subgraph(sub_nodes).copy()
        # Rename the nodes to be contiguous integers
        mapping = {old: new for new, old in enumerate(subgraph.nodes())}
        subgraph = nx.relabel_nodes(subgraph, mapping)
        print(f"Subgraph: {subgraph.number_of_nodes()} nodes, {subgraph.number_of_edges()} edges")

    # Optionally save for inspection
    nx.write_edgelist(subgraph, SUBGRAPH_FILE, data=False)
    print(f"Subgraph written to {SUBGRAPH_FILE}")
    return subgraph

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

def plot_and_save(subgraph, label, filename="subgraph.png"):
    colors = ['red','blue','green', 'yellow', 'orange']
    for i, name in enumerate(subgraph.nodes()):
        subgraph.nodes[name]['color'] = label[name] % len(colors)
    
    # Save the dot file for visualization
    # write_dot(subgraph, "subgraph.dot")
    # Visualize the subgraph with edge weights
    pos=nx.kamada_kawai_layout(subgraph) # pos = nx.nx_agraph.graphviz_layout(G)
    nx.draw_networkx(subgraph,pos, node_color=[subgraph.nodes[n]['color'] for n in subgraph.nodes()], with_labels=False, node_size=10, arrows=True)
    labels = nx.get_edge_attributes(subgraph,'weight')
    nx.draw_networkx_edge_labels(subgraph,pos,edge_labels=labels, font_size=3)
    # nx.draw_kamada_kawai(subgraph, node_color=[subgraph.nodes[n]['color'] for n in subgraph.nodes()], with_labels=False)
    plt.savefig(filename, dpi=300)

def simulate_walkers(graph: nx.Graph):
    walker_traces = dict()

    for node in graph.nodes():
        trace = [node]  # first visit self
        neighbors = list(graph.neighbors(node))
        trace.extend(neighbors)
        walker_traces[node] = trace

    return walker_traces

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



def main():
    download_and_cache()
    full_graph = load_full_graph()
    subgraph = extract_subgraph(full_graph, target_size=300)
    # What is the average number of neighbors for each node in the subgraph?
    print(f"Average degree of subgraph: {sum(dict(subgraph.degree()).values()) / subgraph.number_of_nodes()}")
    avg = 0
    for _ in range(10):
        random_partitioned_label = random_partition(subgraph, num_partitions=5)
        avg += calculate_performance(simulate_walkers(subgraph), random_partitioned_label)
    avg /= 10
    print(f"Average edges crossing partition boundaries for random partitioning: {avg}")
    metis_partitioned_label = metis_partition(subgraph, num_partitions=5)
    print("Metis partitioning performance: ", calculate_performance(simulate_walkers(subgraph), metis_partitioned_label))

    # plot_and_save(subgraph, metis_partitioned_label, filename="subgraph_partitioned.png")
    
if __name__ == "__main__":
    main()
