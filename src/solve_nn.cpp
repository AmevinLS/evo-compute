#include "types.cpp"
#include <algorithm>
#include <climits>
#include <vector>

solution_t solve_nn_end(const tsp_t &tsp,
                        const std::vector<unsigned int> &indices) {
  solution_t solution;

  unsigned int current = indices[0];
  solution.path.push_back(current);
  do {
    unsigned int min = current;
    int min_cost = INT_MAX;

    for (auto idx : indices) {
      if (idx == current) {
        continue;
      }

      if (std::find(solution.path.begin(), solution.path.end(), idx) !=
          solution.path.end()) {
        continue;
      }

      int cost = tsp.adj_matrix(current, idx);

      if (cost < min_cost) {
        min = idx;
        min_cost = cost;
      }
    }

    current = min;
    solution.cost += min_cost;
    solution.path.push_back(min);
  } while (solution.path.size() < indices.size());

  return solution;
}
