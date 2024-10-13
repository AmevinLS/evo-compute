#include "types.cpp"

solution_t solve(const tsp_t &tsp, heuristic_t heuristic) {
  solution_t solution;
  solution.cost = 0;
  solution.path = std::vector<unsigned int>(tsp.n, 0);
  return solution;
}
