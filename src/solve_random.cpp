#pragma once

#include "types.cpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include <vector>

std::vector<solution_t> solve_random(const tsp_t &tsp) {
  int n = ceil(tsp.n / 2);
  std::vector<unsigned int> indices(tsp.n);
  std::iota(indices.begin(), indices.end(), 0);
  std::set<std::vector<unsigned int>> seen;
  std::vector<solution_t> solutions;

  std::stable_sort(indices.begin(), indices.end(),
                   [&tsp](unsigned int a, unsigned int b) {
                     return tsp.nodes[a].weight < tsp.nodes[b].weight;
                   });

  std::random_device rd;
  std::mt19937 g(rd());
  do {
    std::shuffle(indices.begin(), indices.begin() + n, g);
    int cost = 0;
    std::vector<unsigned int> path;

    for (unsigned int i = 0; i < n - 1; i++) {
      cost += tsp.adj_matrix(indices[i], indices[i + 1]);
      path.push_back(indices[i]);
    }
    cost += tsp.adj_matrix(indices[n - 1], indices[0]);
    path.push_back(indices[n - 1]);

    if (seen.find(path) != seen.end()) {
      continue;
    }

    solution_t solution(cost, path);
    solutions.push_back(solution);
    seen.insert(path);
  } while (solutions.size() < tsp.n);

  return solutions;
}

