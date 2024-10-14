#include <cstring>
#include <fstream>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

#include "parse.cpp"
#include "print.cpp"
#include "solve_greedy_cycle.cpp"

#define ERROR "\033[0;31m[ERROR]\033[0m"

enum heuristic_t { RANDOM, GREEDY_CYCLE };

std::string heuristic_to_str(heuristic_t heuristic) {
  switch (heuristic) {
  case RANDOM:
    return "random";
  case GREEDY_CYCLE:
    return "greedycycle";
  default:
    throw std::invalid_argument("Invalid heuristic somehow!");
  }
}

std::vector<solution_t> run_experiments(const tsp_t &tsp,
                                        heuristic_t heuristic) {
  std::vector<solution_t> solutions;
  solution_t solution;
  for (unsigned int starting_node = 0; starting_node < tsp.n; starting_node++) {
    switch (heuristic) {
    case RANDOM:
      solution = {0, {}};
      break;
    case GREEDY_CYCLE:
      solution = run_greedy_cycle(tsp, starting_node);
      break;
    }
    solutions.push_back(solution);
  }
  return solutions;
}

void solutions_to_csv(const std::vector<solution_t> &solutions,
                      std::string csv_path) {
  std::ofstream fout(csv_path);
  fout << "idx,cost,solution" << std::endl;
  for (std::size_t i = 0; i < solutions.size(); i++) {
    fout << i << "," << solutions[i].cost << ",\"[";
    auto path = solutions[i].path;
    for (int i = 0; i < path.size(); i++) {
      fout << path[i];
      if (i != (path.size() - 1)) {
        fout << ",";
      }
    }
    fout << "]\"\n";
  }
}

int main() {
  std::ifstream fin("../data/TSPA.csv");
  tsp_t tspA = parse(fin);

  fin = std::ifstream("../data/TSPB.csv");
  tsp_t tspB = parse(fin);

  for (auto heuristic : {RANDOM, GREEDY_CYCLE}) {
    auto solutions = run_experiments(tspA, heuristic);
    solutions_to_csv(solutions,
                     "../results/tspa_" + heuristic_to_str(heuristic) + ".csv");

    solutions = run_experiments(tspB, heuristic);
    solutions_to_csv(solutions,
                     "../results/tspb_" + heuristic_to_str(heuristic) + ".csv");
  }
}