node Data {
  x: int[SIZE];
  y: int[SIZE];
  centroid: int[SIZE];
}

walker kmeans_walk {
  centroidx: float[K];
  centroidy: float[K];
  can nearest_centroids with Data entry {
    for i in range(SIZE) {
      min_dist = float("inf");
      min_centroid = 0;
      for j in range(K) {
        dist = (centroidx[j] - here.x[i]) ** 2 + (centroidy[j] - here.y[i]) ** 2;
        if min_dist > dist {
          min_dist = dist;
          min_centroid = j;
        }
        here.centroid[i] = min_centroid;
      }
    }
  }
}


with entry {
  // Randomize centroids
  centroidx = [];
  centroidy = [];
  for i in rnage(K) {
    centroidx.append(rand());
    centroidy.append(rand());
  }
  while true {
    for i in range(N) {
      wlk[i] = aync data[i] spawn kmeans_walk(centroidx, centroidy);
    }
    for i in range(N) {
      wait wlk[i];
    }
    // Recalculate the centroids
    count = centroidy = centroidx = [0] * K;
    for i in range(N) {
      for j in range(SIZE) {
        count[data[i].centroid[j]] += 1;
        centroidx[data[i].centroid[j]] += data[i].x[j];
        centroidy[data[i].centroid[j]] += data[i].y[j];
      }
    }
    
    for i in range(K) {
      centroidx[i] /= count[i];
      centroidy[i] /= count[i];
    }
  }
}
