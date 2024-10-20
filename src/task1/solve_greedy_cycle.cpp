#include "../common/search.cpp"
#include "../common/types.cpp"

#include <climits>
#include <optional>
#include <vector>

solution_t solve_greedy_cycle(const tsp_t &tsp, unsigned int n,
                              unsigned int start) {
  // Select first 3 nodes "optimally"
  solution_t solution(tsp, find_cycle(tsp, start));

  // Select the remaining nodes using GreedyCycle
  while (solution.path.size() < n) {
    std::optional<unsigned int> min;
    int min_cost = INT_MAX;
    int idx = 0;

    for (int i = 0; i < solution.path.size(); i++) {
      auto [min_replace, min_replace_cost] = find_replace(tsp, solution, i);

      if (min_replace_cost < min_cost) {
        min_cost = min_replace_cost;
        min = min_replace;
        idx = i;
      }
    }

    solution.insert(min.value(), idx);
  }

  return solution;
}
