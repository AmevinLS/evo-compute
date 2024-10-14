#include "types.cpp"
#include <algorithm>
#include <climits>
#include <iostream>
#include <ostream>
#include <vector>

std::pair<unsigned int, int>
find_nn(const tsp_t &tsp, const solution_t &solution, unsigned int current) {
  unsigned int min = current;
  int min_dist = INT_MAX;

  for (int idx = 0; idx < tsp.n; idx++) {
    if (idx == current) {
      continue;
    }

    if (std::find(solution.path.begin(), solution.path.end(), idx) !=
        solution.path.end()) {
      continue;
    }

    int dist = tsp.adj_matrix(current, idx) + tsp.nodes[idx].weight;

    if (dist < min_dist) {
      min = idx;
      min_dist = dist;
    }
  }

  return {min, min_dist};
}

solution_t solve_nn_end(const tsp_t &tsp, unsigned int n, unsigned int start) {
  solution_t solution(tsp.nodes[start].weight, {start});

  do {
    auto [min, dist] = find_nn(tsp, solution, solution.path.back());

    solution.cost += dist;
    solution.path.push_back(min);
  } while (solution.path.size() < n);

  solution.cost += tsp.adj_matrix(solution.path.back(), solution.path.front());
  return solution;
}

solution_t solve_nn_any(const tsp_t &tsp, unsigned int n, unsigned int start) {
  solution_t solution(tsp.nodes[start].weight, {start});

  do {
    unsigned int min = solution.path.back();
    unsigned int idx = -1;
    int min_dist = INT_MAX;

    for (int i = 0; i < solution.path.size() - 1; i++) {
      unsigned int a = solution.path[i];
      unsigned int b = solution.path[i + 1];

      auto [a_min, dist] = find_nn(tsp, solution, a);

      dist += tsp.adj_matrix(a_min, b) - tsp.adj_matrix(a, b);

      if (dist) {
        min = a_min;
        min_dist = dist;
        idx = i;
      }
    }

    auto [last_min, dist] = find_nn(tsp, solution, solution.path.back());
    if (dist < min_dist) {
      solution.path.push_back(last_min);
      min_dist = dist;
    } else {
      solution.path.insert(solution.path.begin() + idx + 1, min);
    }

    solution.cost += min_dist;
  } while (solution.path.size() < n);

  solution.cost += tsp.adj_matrix(solution.path.back(), solution.path.front());

  return solution;
}
