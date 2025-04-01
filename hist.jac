node Data {
  has chunk: float[SIZE];
}

node End {
  has result: int[BUCKET_NUM];
}

walker histogram {
  has tmp_result: int[BUCKET_NUM];
  can get with entry Data {
    for i in range(BUCKET) {
      self.tmp_result[i] = 0;
    }
    for data in here.chunk {
      self.tmp_result[int(data)]++;
    }
    visit [-->];
  }

  can sum_up with entry End {
    for i in range(BUCKET_NUM) {
      here.result[i] += tmp_result[i];
    }
  }
}
