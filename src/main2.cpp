#include "parse.cpp"
#include "print.cpp"
#include "solve.cpp"
#include "types.cpp"
#include <fstream>

int main() {
  std::string fname = "../data/TSPA.csv";
  std::ifstream fin(fname);

  if (!fin.is_open()) {
    return 1;
  }

  tsp_t tsp = parse(fin);
  std::string instance_name = fname.substr(fname.find_last_of("/\\") + 1);
  instance_name = instance_name.substr(0, instance_name.find_last_of("."));

  for (auto &[key, value] : heuristic_t_str) {
    std::vector solutions = solve(tsp, key);
    std::ofstream out(instance_name + "_" + value + ".csv");

    out << solutions;
  }

  return 0;
}
