node Data {
  has chunk: float[1000];
}

node End {
  has result: list[int] = [0] * 100;
}

walker histogram {
  has tmp_result: list[int] = [0] * 100;
  can get with Data entry {
    for data in here.chunk {
      self.tmp_result[int(data)]+= 1;
    }
    visit [-->];
  }

  can sum_up with End entry {
    for i in range(100) {
      here.result[i] += self.tmp_result[i];
    }
  }
}

with entry {
  data1 = Data(chunk=[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]);
  data2 = Data(chunk=[11, 12, 13, 14, 15, 16, 17, 18, 19, 20]);
  data3 = Data(chunk=[21, 22, 23, 24, 25, 26, 27, 28, 29, 30]);
  data4 = Data(chunk=[31, 32, 33, 34, 35, 36, 37, 38, 39, 40]);
  end = End();

  data1 ++> data2;
  data2 ++> end;
  data3 ++> data4;
  data4 ++> end;
  wlk1 = data1 spawn histogram();
  wlk2 = data3 spawn histogram();
  print(end.result);
}
