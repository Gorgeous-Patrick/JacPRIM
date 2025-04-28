# Reduce than scan
node Data {
  has data: int;
  has remaining_visits: int;
  has lower: int = 0;
  has upper: int = 0;
}
edge Down {}
edge Up{}

walker red {
  has result: int = 0;
  can red with Data entry {
    print(self.result);
    original = here.data;
    here.data += self.result;
    # print(f"{original} + {self.result} = {here.data}");
    self.result = here.data;

    here.remaining_visits -= 1;
    if here.remaining_visits == 0 {
      print("Leaving: ", len([-:Up:->]));
      visit [-:Up:->];
    }
  }
}

walker scan {
  has pnt_lower: int;
  has pnt_upper: int;
  has pnt: bool = True;
  has left: bool = True;
  has right: bool = False;

  can scan with Data entry {
    if self.pnt {
      self.pnt_lower = here.lower;
      self.pnt_upper = here.upper;
      self.pnt = False;
      self.left = True;
      self.right = False;
      visit [-:Down:->];
    }
    if self.left {
      here.lower = self.pnt_lower;
      here.upper = self.pnt_lower + here.data;
      self.pnt_lower = here.upper;
      self.left = False;
      self.right = True;
      wlk = here spawn scan(pnt_lower = here.lower, pnt_upper = here.upper);

    }
    if self.right {
      here.upper = self.pnt_lower + here.data;
      self.right = False;
      wlk = here spawn scan(pnt_lower = here.lower, pnt_upper = here.upper);
    }

  }
}


with entry {
  data = [1, 2, 3, 4, 5];
  frontiers = [[]];

  for i in range(len(data)) {
    data[i] = Data(data=data[i], remaining_visits=1);
    frontiers[0].append(data[i]);
  }
  while len(frontiers[-1]) > 1 {
    frontier = frontiers[-1];
    new_frontier = [];
    for i in range(len(frontier) // 2) {
      d = Data(data=0, remaining_visits=2);
      frontier[i * 2] +:Up:+> d;
      frontier[i * 2 + 1] +:Up:+> d;
      print("Data ", frontier[i * 2].data, frontier[i * 2 + 1].data);
      d +:Down:+> frontier[i * 2];
      d +:Down:+> frontier[i * 2 + 1];
      new_frontier.append(d);
    }
    if len(frontier) % 2 == 1 {
      d = Data(data=0, remaining_visits=1);
      frontier[-1] +:Up:+> d;
      d +:Down:+> frontier[-1];

      new_frontier.append(d);
    }
    frontiers.append(new_frontier);
  }

  for i in range(len(data)){
    print("================");

    for frontier in frontiers {
      print("Frontier ", [f"{f.data}" for f in frontier]);
    }
    print("Original ", data[i].data);
    wlk = data[i] spawn red();
  }
  print("================");

  for frontier in frontiers {
    print("Frontier ", [f"{f.data}" for f in frontier]);
  }

  wlk = frontiers[-1][0] spawn scan(pnt_lower=0, pnt_upper=frontiers[-1][0].data);
  

}
