#pragma once

#include <map>
#include <math.h>
#include <optional>
#include <set>
#include <vector>

#include "types.cpp"

int get_cost_diff(const tsp_t &tsp, const std::vector<unsigned int> &nodes,
                  unsigned int new_node, int position) {
  int pre_node = nodes[position];
  int post_node;
  if (position == (nodes.size() - 1)) {
    post_node = nodes[0];
  } else {
    post_node = nodes[position + 1];
  }
  return tsp.nodes.at(new_node).weight - tsp.adj_matrix.m[pre_node][post_node] +
         tsp.adj_matrix.m[pre_node][new_node] +
         tsp.adj_matrix.m[new_node][post_node];
}

solution_t run_greedy_cycle(const tsp_t &tsp, unsigned int starting_node) {
  std::size_t num_nodes_left = (size_t)round(tsp.n / 2.0);
  std::set<unsigned int> remaining_nodes;
  for (int i = 0; i < tsp.n; i++) {
    remaining_nodes.insert(i);
  }

  std::vector<unsigned int> nodes;

  std::optional<int> best_cost;
  for (auto i : remaining_nodes) {
    for (auto j : remaining_nodes) {
      if (i == j || i == starting_node || j == starting_node)
        continue;
      int candidate_cost =
          tsp.nodes[i].weight + tsp.nodes[j].weight +
          tsp.nodes[starting_node].weight + tsp.adj_matrix.m[starting_node][i] +
          tsp.adj_matrix.m[i][j] + tsp.adj_matrix.m[j][starting_node];
      if (!best_cost.has_value() || candidate_cost < best_cost.value()) {
        nodes = {starting_node, i, j};
        best_cost = candidate_cost;
      }
    }
  }
  for (auto node : nodes) {
    remaining_nodes.erase(node);
  }

  int cost = best_cost.value();

  while (num_nodes_left > 0) {
    std::optional<int> selected_node, min_diff;
    for (int node : remaining_nodes) {
      for (int pos = 0; pos < nodes.size(); pos++) {
        int diff = get_cost_diff(tsp, nodes, node, pos);
        if (!selected_node.has_value() || diff < min_diff) {
          min_diff = diff;
          selected_node = node;
        }
      }
    }
    nodes.push_back(selected_node.value());
    cost += min_diff.value();
    remaining_nodes.erase(selected_node.value());
    num_nodes_left--;
  }
  return {cost, nodes};
}