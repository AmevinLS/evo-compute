#include "types.cpp"

#include <vector>

enum heuristic_t { RANDOM };

std::vector<solution_t> init(const tsp_t &tsp) {
  std::vector<solution_t> solutions;

  for (unsigned int i = 0; i < tsp.n; i++) {
    solutions.push_back(solution_t(0, {i}));
  }

  return solutions;
}

std::vector<solution_t> solve(const tsp_t &tsp, heuristic_t heuristic) {
  std::vector<solution_t> solutions = init(tsp);

  switch (heuristic) {
  case RANDOM:
    return solutions;
  }

  return {};
}
