#pragma once

#include "types.cpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include <vector>

std::vector<solution_t> solve_random(const tsp_t &tsp, unsigned int n) {
  std::vector<unsigned int> indices(tsp.n);
  std::iota(indices.begin(), indices.end(), 0);
  std::set<std::vector<unsigned int>> seen;
  std::vector<solution_t> solutions;

  std::random_device rd;
  std::mt19937 g(rd());
  do {
    std::shuffle(indices.begin(), indices.end(), g);
    std::vector<unsigned int> path(indices.begin(), indices.begin() + n);

    if (seen.find(path) != seen.end()) {
      continue;
    }

    int cost = tsp.adj_matrix(path.back(), path.front()) +
               tsp.nodes[path.back()].weight;
    for (int i = 0; i < n - 1; i++) {
      cost += tsp.adj_matrix(indices[i], indices[i + 1]) +
              tsp.nodes[indices[i]].weight;
    }

    solution_t solution(cost, path);
    solutions.push_back(solution);
    seen.insert(path);
  } while (solutions.size() < tsp.n);

  return solutions;
}
