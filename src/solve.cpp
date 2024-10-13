#include "solve_random.cpp"
#include "types.cpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>

enum heuristic_t { RANDOM };

std::vector<node_t> neighbors(const tsp_t &tsp, unsigned int start) {
  int n = ceil(tsp.n / 2);
  adj_list_t neighbors = tsp.adj_matrix[start];
  std::vector<unsigned int> indices(neighbors.size());
  std::iota(indices.begin(), indices.end(), 0);
  std::vector<node_t> nodes;

  std::stable_sort(indices.begin(), indices.end(),
                   [&neighbors](unsigned int a, unsigned int b) {
                     return neighbors[a] < neighbors[b];
                   });

  std::transform(indices.begin(), indices.begin() + n,
                 std::back_inserter(nodes),
                 [&tsp](unsigned int i) { return tsp.nodes[i]; });

  return nodes;
}

std::vector<solution_t> solve(const tsp_t &tsp, heuristic_t heuristic) {
  std::vector<solution_t> solutions;

  if (heuristic == RANDOM) {
    return solve_random(tsp);
  }

  return {};
}
