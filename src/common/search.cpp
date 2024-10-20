#pragma once

#include "types.cpp"

#include <climits>
#include <optional>
#include <utility>
#include <vector>

typedef std::pair<unsigned int, int> best_t;

best_t find_nn(const tsp_t &tsp, const solution_t &solution,
               unsigned int prev) {
  std::optional<unsigned int> min;
  int min_cost = INT_MAX;

  for (auto next : solution.remaining_nodes) {
    int cost = tsp.adj_matrix(prev, next) + tsp.weights[next];

    if (cost < min_cost || !min.has_value()) {
      min = next;
      min_cost = cost;
    }
  }

  return {min.value(), min_cost};
}

best_t find_replace(const tsp_t &tsp, const solution_t &solution, int pos) {
  std::optional<unsigned int> min;
  int min_cost = INT_MAX;

  for (auto node : solution.remaining_nodes) {
    int cost = solution.get_cost_diff(node, pos);

    if (cost < min_cost || !min.has_value()) {
      min = node;
      min_cost = cost;
    }
  }

  return {min.value(), min_cost};
}

std::vector<unsigned int> find_cycle(const tsp_t &tsp, unsigned int start) {
  std::optional<std::vector<unsigned int>> path;
  int min_cost = INT_MAX;

  for (unsigned int i = 0; i < tsp.n; i++) {
    if (i == start) {
      continue;
    }

    for (unsigned int j = 0; j < tsp.n; j++) {
      if (j == start || i == j) {
        continue;
      }

      int cost = tsp.weights[i] + tsp.weights[j] + tsp.weights[start] +
                 tsp.adj_matrix(start, i) + tsp.adj_matrix(i, j) +
                 tsp.adj_matrix(j, start);

      if (cost < min_cost || !path.has_value()) {
        min_cost = cost;
        path = {start, i, j};
      }
    }
  }

  return path.value();
}
