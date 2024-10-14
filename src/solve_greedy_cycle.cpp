#pragma once

#include "types.cpp"
#include <climits>
#include <set>
#include <vector>

int get_cost_diff(const tsp_t &tsp, const std::vector<unsigned int> &nodes,
                  unsigned int new_node, int position) {
  int pre_node = nodes[position];
  int post_node;
  if (position == (nodes.size() - 1)) {
    post_node = nodes[0];
  } else {
    post_node = nodes[position + 1];
  }
  return tsp.nodes[new_node].weight - tsp.adj_matrix.m[pre_node][post_node] +
         tsp.adj_matrix.m[pre_node][new_node] +
         tsp.adj_matrix.m[new_node][post_node];
}

solution_t run_greedy_cycle(const tsp_t &tsp, unsigned int n,
                            unsigned int start) {
  solution_t solution(INT_MAX, {});
  std::set<unsigned int> remaining_nodes;

  for (int i = 0; i < tsp.n; i++) {
    remaining_nodes.insert(i);
  }

  // Select first 3 nodes "optimally"
  for (auto i : remaining_nodes) {
    for (auto j : remaining_nodes) {
      if (i == j || i == start || j == start) {
        continue;
      }

      int candidate_cost = tsp.nodes[i].weight + tsp.nodes[j].weight +
                           tsp.nodes[start].weight +
                           tsp.adj_matrix.m[start][i] + tsp.adj_matrix.m[i][j] +
                           tsp.adj_matrix.m[j][start];

      if (candidate_cost < solution.cost) {
        solution.path = {start, i, j};
        solution.cost = candidate_cost;
      }
    }
  }
  for (auto node : solution.path) {
    remaining_nodes.erase(node);
  }

  // Select the remaining nodes using GreedyCycle
  while (solution.path.size() < n) {
    int min_diff = INT_MAX;
    int idx = 0;
    unsigned int selected_node = 0;

    for (int node : remaining_nodes) {
      for (int i = 0; i < solution.path.size(); i++) {
        int diff = get_cost_diff(tsp, solution.path, node, i);

        if (diff < min_diff) {
          min_diff = diff;
          selected_node = node;
          idx = i;
        }
      }
    }

    solution.path.insert(solution.path.begin() + idx + 1, selected_node);
    solution.cost += min_diff;
    remaining_nodes.erase(selected_node);
  }

  return solution;
}
