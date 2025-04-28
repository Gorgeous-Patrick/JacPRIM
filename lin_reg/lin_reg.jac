node Data {
  chunk: float [SIZE][DIM];
  y_true: float[SIZE];
}

walker lin_reg {
  has weights: float [SIZE][DIM];
  can gradient with entry Data {
    output = [0] * DIM;
    for j in range(DIM) {
      output[j] = 0;
    }

    for i in range(SIZE) {
      y_pred = 0;
      for j in range(DIM) {
        y_pred += weights[i][j] * here.chunk[i][j];
      }
      error = y_pred - y_true[i];
      for j in range(DIM) {
        output[j] += 2 * error * here.chunk[i][j];
      }
    }
  }
}

