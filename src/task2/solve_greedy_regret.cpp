#pragma once

#include <algorithm>
#include <climits>
#include <numeric>
#include <set>
#include <stdexcept>

#include "../common/types.cpp"
#include "../task1/solve_greedy_cycle.cpp"

unsigned total_cost(const tsp_t &tsp, std::vector<unsigned> path) {
    unsigned cost = 0;
    for (int i = 0; i < path.size(); i++) {
        unsigned curr_node = path[i];
        unsigned next_node = (i == path.size() - 1) ? path[0] : path[i + 1];
        cost +=
            tsp.nodes[curr_node].weight + tsp.adj_matrix(curr_node, next_node);
    }
    return cost;
}

class CycleRegretSolver {
  private:
    const tsp_t &tsp;
    std::set<unsigned> remaining_nodes;
    solution_t solution;

    void add_starting_cycle(unsigned start) {
        // Select first 3 nodes "optimally"
        for (auto i : remaining_nodes) {
            for (auto j : remaining_nodes) {
                if (i == j || i == start || j == start) {
                    continue;
                }

                int candidate_cost =
                    tsp.nodes[i].weight + tsp.nodes[j].weight +
                    tsp.nodes[start].weight + tsp.adj_matrix.m[start][i] +
                    tsp.adj_matrix.m[i][j] + tsp.adj_matrix.m[j][start];

                if (candidate_cost < solution.cost) {
                    solution.path = {start, i, j};
                    solution.cost = candidate_cost;
                }
            }
        }
        for (auto node : solution.path) {
            remaining_nodes.erase(node);
        }
    }

    void reset() {
        solution = {INT_MAX, {}};
        remaining_nodes = std::set<unsigned>();
        for (int i = 0; i < tsp.n; i++) {
            remaining_nodes.insert(i);
        }
    }

  public:
    CycleRegretSolver(const tsp_t &tsp) : tsp(tsp) { reset(); }

    solution_t solve(unsigned n, unsigned start, unsigned regret_k,
                     float regret_weight) {
        if (regret_weight < 0.0 || regret_weight > 1.0)
            throw std::invalid_argument("regret_k must be between 0 and 1");

        reset();

        add_starting_cycle(start);

        // Select the remaining nodes using GreedyCycle
        std::vector<std::pair<unsigned, int>> cost_diffs;
        cost_diffs.reserve(tsp.n);
        while (solution.path.size() < n) {
            int max_score = INT_MIN;
            unsigned best_pos = 0;
            unsigned best_node = 0;
            for (unsigned node : remaining_nodes) {
                cost_diffs.clear();
                for (int pos = 0; pos < solution.path.size(); pos++) {
                    int diff = get_cost_diff(tsp, solution.path, node, pos);
                    cost_diffs.push_back({pos, diff});
                }
                std::sort(cost_diffs.begin(), cost_diffs.end(),
                          [](const auto &pair1, const auto &pair2) {
                              return pair1.second < pair2.second;
                          });
                int regret = 0;
                std::for_each(cost_diffs.begin(), cost_diffs.begin() + regret_k,
                              [&regret, cost_diffs](
                                  const std::pair<unsigned, int> &pair) {
                                  regret += pair.second - cost_diffs[0].second;
                              });
                int score = regret_weight * regret -
                            (1 - regret_weight) * cost_diffs[0].second;
                if (regret > max_score) {
                    max_score = score;
                    best_node = node;
                    best_pos = cost_diffs[0].first;
                }
            }

            solution.path.insert(solution.path.begin() + best_pos + 1,
                                 best_node);
            remaining_nodes.erase(best_node);
        }
        solution.cost = total_cost(tsp, solution.path);
        return solution;
    }
};