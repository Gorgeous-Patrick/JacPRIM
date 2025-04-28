node Data {
  has chunk: float[SIZE];
  has result: float = 0;
}

edge FinalEdge {}


walker regular {
  has result: float = 0;
  can sum_up with Data entry{
    self.result += sum(here.chunk);
    here.result = self.result;
    visit [-->];
  }
}

walker final {
  has result: float = 0;
  can sum_up with Data entry {
    self.result += here.result;
    visit [-:FinalEdge:->];
  }
}

with entry {
  data1 = Data(chunk=[1, 2, 3, 4, 5, 6, 7, 8, 9, 10]);
  data2 = Data(chunk=[11, 12, 13, 14, 15, 16, 17, 18, 19, 20]);
  data3 = Data(chunk=[21, 22, 23, 24, 25, 26, 27, 28, 29, 30]);
  data4 = Data(chunk=[31, 32, 33, 34, 35, 36, 37, 38, 39, 40]);
  data5 = Data(chunk=[41, 42, 43, 44, 45, 46, 47, 48, 49, 50]);
  data6 = Data(chunk=[51, 52, 53, 54, 55, 56, 57, 58, 59, 60]);
  
  data1 ++> data2;
  data2 ++> data3;

  data4 ++> data5;
  data5 ++> data6;

  data3 +:FinalEdge:+> data6;
  
  wlk1 = data1 spawn regular();
  wlk2 = data4 spawn regular();

  end = data3 spawn final();
  print(end.result);
}
