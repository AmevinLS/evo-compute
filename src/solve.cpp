#include "solve_nn.cpp"
#include "solve_random.cpp"
#include "types.cpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

enum heuristic_t { RANDOM, NN_END };

std::vector<unsigned int> constrain(const tsp_t &tsp, unsigned int start) {
  int n = ceil(tsp.n / 2);
  adj_list_t neighbors = tsp.adj_matrix[start];
  std::vector<unsigned int> indices(neighbors.size());
  std::iota(indices.begin(), indices.end(), 0);

  std::stable_sort(indices.begin(), indices.end(),
                   [&neighbors](unsigned int a, unsigned int b) {
                     return neighbors[a] < neighbors[b];
                   });

  return std::vector<unsigned int>(indices.begin(), indices.begin() + n);
}

std::vector<solution_t> solve(const tsp_t &tsp, heuristic_t heuristic) {
  std::vector<solution_t> solutions;

  if (heuristic == RANDOM) {
    return solve_random(tsp);
  }

  for (unsigned int i = 0; i < tsp.n; i++) {
    std::vector<unsigned int> indices = constrain(tsp, i);

    switch (heuristic) {
    case RANDOM:
      // NOTE: should never reach this point, but it's here to avoid a warning
      break;
    case NN_END:
      solutions.push_back(solve_nn_end(tsp, indices));
    }
  }

  return solutions;
}
