from data_prep import download_and_cache, get_subgraph
import os

SNAP_URL = "https://snap.stanford.edu/data/twitter-2010.txt.gz"
DATA_DIR = "snap_data"
RAW_FILE = os.path.join(DATA_DIR, "twitter.txt.gz")
EXTRACTED_FILE = os.path.join(DATA_DIR, "twitter.txt")
SUBGRAPH_FILE = os.path.join(DATA_DIR, "twitter-subgraph.txt")

def main():
    download_and_cache(RAW_FILE, EXTRACTED_FILE, SNAP_URL, DATA_DIR)
    G = get_subgraph(EXTRACTED_FILE, SUBGRAPH_FILE, target_size=1000, limit=100000)

if __name__ == "__main__":
    main()