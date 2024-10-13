#include <iostream>

#include "parse.cpp"
#include "print.cpp"

int main() {
  std::ifstream in("./data/TSPA.csv");
  tsp_t tsp = parse(in);

  std::cout << tsp.adj_matrix << std::endl;
  return 0;
}
