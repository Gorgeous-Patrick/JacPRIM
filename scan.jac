node Chunk {
  data: int[SIZE];
  result: int[SIZE];
};

walker sum {
  int sum;
  can sum_up_all with entry Chunk {
    sum += sum(here.data);
    if ([-->].empty()) {
      report sum;
    } else {
      visit [-->];
    }
  }
}

walker subtract {
  int sum;
  can get_result with entry Chunk {
    for i in SIZE {
      result[i] = sum;
      sum -= result[i];
    }
    if ([-->].empty()) {
      report sum;
    } else {
      visit [-->];
    }
  }
}
