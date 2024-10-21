#include "../common/search.cpp"
#include "../common/types.cpp"

solution_t solve_nn_end(const tsp_t &tsp, unsigned int n, unsigned int start) {
  solution_t solution(tsp, start);

  while (solution.path.size() < n) {
    best_t min = find_nn(tsp, solution, solution.path.back());

    solution.append(min.first);
  }

  solution.commit();
  return solution;
}

solution_t solve_nn_any(const tsp_t &tsp, unsigned int n, unsigned int start) {
  solution_t solution(tsp, start);

  while (solution.path.size() < n) {
    unsigned int min_pos = -1;
    auto [min, min_cost] = find_nn(tsp, solution, solution.path.front());

    for (int i = 0; i < solution.path.size() - 1; i++) {
      auto [min_replace, min_replace_cost] = find_replace(tsp, solution, i);

      if (min_replace_cost < min_cost) {
        min = min_replace;
        min_cost = min_replace_cost;
        min_pos = i;
      }
    }

    auto [last, last_cost] = find_nn(tsp, solution, solution.path.back());
    if (last_cost < min_cost) {
      solution.append(last);
    } else if (min_pos == -1) {
      solution.prepend(min);
    } else {
      solution.insert(min, min_pos);
    }
  }

  solution.commit();
  return solution;
}
