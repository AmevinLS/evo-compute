#include "solve_nn.cpp"
#include "solve_random.cpp"
#include "types.cpp"

#include <vector>

enum heuristic_t { RANDOM, NN_END, NN_ANY };

std::vector<solution_t> solve(const tsp_t &tsp, heuristic_t heuristic) {
  std::vector<solution_t> solutions;
  unsigned int n = ceil(tsp.n / 2.0);

  if (heuristic == RANDOM) {
    return solve_random(tsp, n);
  }

  for (unsigned int i = 0; i < tsp.n; i++) {
    switch (heuristic) {
    case RANDOM:
      // NOTE: should never reach this point, but it's here to avoid a warning
      break;
    case NN_END:
      solutions.push_back(solve_nn_end(tsp, n, i));
      break;
    case NN_ANY:
      solutions.push_back(solve_nn_any(tsp, n, i));
      break;
    }
  }

  return solutions;
}
