node Data {
  has chunk: float[SIZE];
  has result: float;
}

edge FinalEdge {}


walker regular {
  has result: float;
  can sum_up with entry Data {
    self.result += sum(here.chunk);
    here.result = self.result;
    visit [-->];
  }
}

walker final {
  has result: float;
  can sum_up with entry Data {
    self.result += here.result;
    visit [-[FinalEdge]->];
  }
}


