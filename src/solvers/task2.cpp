#pragma once

#include "../common/search.cpp"
#include "../common/types.cpp"

#include <algorithm>
#include <climits>
#include <numeric>

#define REGRET_K 2
#define REGRET_WEIGHT 0.5

solution_t greedy_cycle_regret(solution_t solution, float weight) {
    std::vector<pos_delta_t> deltas;
    deltas.reserve(solution.tsp->path_size);

    while (solution.path.size() < solution.tsp->path_size) {
        std::optional<unsigned int> max;
        int max_score = INT_MIN;
        int idx = 0;

        for (auto node : solution.remaining_nodes) {
            deltas.clear();

            for (int i = 0; i < solution.path.size(); i++) {
                deltas.push_back({i, solution.insert_delta(node, i)});
            }

            std::sort(deltas.begin(), deltas.end(),
                      [](const pos_delta_t &pair1, const pos_delta_t &pair2) {
                          return pair1.second < pair2.second;
                      });

            int regret = std::accumulate(
                deltas.begin(), deltas.begin() + REGRET_K, 0,
                [&](int regret, pos_delta_t pair) {
                    return regret + pair.second - deltas[0].second;
                });

            int score = weight * regret - (1 - weight) * deltas[0].second;

            if (score > max_score) {
                max_score = score;
                max = node;
                idx = deltas[0].first;
            }
        }

        solution.insert(max.value(), idx);
    }

    return solution;
}
