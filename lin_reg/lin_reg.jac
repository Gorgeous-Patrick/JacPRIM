node Data {
  has chunk: list[list[float]];
  has y_true: list[float];
}

walker lin_reg {
  has weights: list[list[float]];
  has output: list[float] = [];
  can gradient with Data entry {
    DIM = len(self.weights[0]);
    SIZE = len(here.chunk);

    for i in range(SIZE) {
      y_pred = 0;
      for j in range(DIM) {
        y_pred += self.weights[i][j] * here.chunk[i][j];
      }
      error = y_pred - here.y_true[i];
      for j in range(DIM) {
        self.output[j] += 2 * error * here.chunk[i][j];
      }
    }
    visit [-->];
  }
}

with entry {
  data1 = Data(chunk=[[1, 2, 3], [4, 5, 6], [7, 8, 9]], y_true=[1, 2, 3]);
  data2 = Data(chunk=[[1, 2, 3], [4, 5, 6], [7, 8, 9]], y_true=[1, 2, 3]);
  data3 = Data(chunk=[[1, 2, 3], [4, 5, 6], [7, 8, 9]], y_true=[1, 2, 3]);
  data4 = Data(chunk=[[1, 2, 3], [4, 5, 6], [7, 8, 9]], y_true=[1, 2, 3]);
  data1 ++> data2;
  data2 ++> data3;
  data3 ++> data4;

  output = [0] * len(data1.chunk[0]);
  weights = [[0.1, 0.2, 0.3], [0.4, 0.5, 0.6], [0.7, 0.8, 0.9]];
  wlk = data1 spawn lin_reg(weights=weights, output=output);
  print(wlk.output);
}