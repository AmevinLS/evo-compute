#include "types.cpp"
#include <algorithm>
#include <climits>
#include <vector>

unsigned int find_nn(const tsp_t &tsp, const std::vector<unsigned int> &indices,
                     const solution_t &solution) {
  unsigned int current = solution.path.back();
  unsigned int min = current;
  int min_dist = INT_MAX;

  for (auto idx : indices) {
    if (idx == current) {
      continue;
    }

    if (std::find(solution.path.begin(), solution.path.end(), idx) !=
        solution.path.end()) {
      continue;
    }

    int dist = tsp.adj_matrix(current, idx);

    if (dist < min_dist) {
      min = idx;
      min_dist = dist;
    }
  }

  return min;
}

solution_t solve_nn_end(const tsp_t &tsp,
                        const std::vector<unsigned int> &indices) {
  solution_t solution(0, {indices[0]});

  do {
    unsigned int min = find_nn(tsp, indices, solution);
    solution.cost +=
        tsp.adj_matrix(solution.path.back(), min) + tsp.nodes[min].weight;
    solution.path.push_back(min);
  } while (solution.path.size() < indices.size());

  solution.cost += tsp.adj_matrix(solution.path.back(), solution.path.front());
  return solution;
}
