#include "types.cpp"

enum heuristic_t { RANDOM };

solution_t solve(const tsp_t &tsp, heuristic_t heuristic) {
  switch (heuristic) {
  case RANDOM:
    return solution_t();
  }

  return solution_t();
}
