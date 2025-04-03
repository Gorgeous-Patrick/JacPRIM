node Data {
  input: int[DIM];
  truth: int;
}

node Acc {
  new_weight: int[DIM];
}


walker log_reg_train {
  weight: int[DIM];
  grads: int[DIM];
  can grad_calc with entry Data {
     dot = 0;
     for i in range(DIM) {
      dot += this.weight[i] * here.input[i];
     }
     error = sigmoid(dot) - here.truth;
     for i in range(DIM) {
      grads[i] = error * here.input[i];
     }
  }

  can accumulate with entry Acc {
     for i in range(DIM) {
      here.new_weight[i] += this.grads[i];
     }
  }
}
