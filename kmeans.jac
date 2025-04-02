node Data {
  x: float[SIZE];
  y: float[SIZE];
  centroid: int[SIZE];
};

walker kmeans_walk {
  centroidx: float[K];
  centroidy: float[K];
  can nearest_centroids with Data entry {
    for i in range(SIZE) {
      min_dist = float("inf");
      min_centroid = 0;
      for j in range(K) {
        dist = (centroidx[j] - here.x[i]) ** 2 + (centroidy[j] - here.y[i])
        if min_dist > dist {
          min_dist = dist;
          min_centroid = j;
        }
        here.centroid[i] = min_centroid;
      }
    }
  }
};


with entry {
  // Async Spawn multiple walkers
  // Wait for them
  // Collect the results
  // Recalculate the centroids
}
