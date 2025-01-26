#pragma once

#include "../common/search.cpp"
#include "../common/types.cpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include <vector>

std::vector<solution_t> random(const tsp_t &tsp) {
    std::vector<unsigned int> indices(tsp.n);
    std::iota(indices.begin(), indices.end(), 0);
    std::set<std::vector<unsigned int>> seen;
    std::vector<solution_t> solutions;
    timer_t timer;
    std::mt19937 g((std::random_device()()));

    while (solutions.size() < tsp.n) {
        timer.start();
        std::shuffle(indices.begin(), indices.end(), g);
        std::vector<unsigned int> path(indices.begin(),
                                       indices.begin() + tsp.path_size);

        if (seen.find(path) != seen.end()) {
            continue;
        }

        solutions.push_back(solution_t(tsp, path, timer.measure()));
        seen.insert(path);
    }

    return solutions;
}

solution_t nn_end(const tsp_t &tsp, unsigned int start) {
    solution_t solution(tsp, start);

    while (solution.path.size() < tsp.path_size) {
        auto [min, min_delta] = find_nn(solution, [=](unsigned int node) {
            return solution.append_delta(node);
        });

        solution.append(min);
    }

    solution.cost +=
        tsp.adj_matrix[solution.path.back()][solution.path.front()];
    return solution;
}

solution_t nn_any(const tsp_t &tsp, unsigned int start) {
    solution_t solution(tsp, start);

    while (solution.path.size() < tsp.path_size) {
        unsigned int min_pos = -1;

        auto [min, min_delta] = find_nn(solution, [=](unsigned int node) {
            return solution.prepend_delta(node);
        });

        for (int i = 0; i < solution.path.size() - 1; i++) {
            auto [min_replace, min_replace_delta] = find_replace(solution, i);

            if (min_replace_delta < min_delta) {
                min = min_replace;
                min_delta = min_replace_delta;
                min_pos = i;
            }
        }

        auto [last, last_delta] = find_nn(solution, [=](unsigned int node) {
            return solution.append_delta(node);
        });

        if (last_delta < min_delta) {
            solution.append(last);
        } else if (min_pos == -1) {
            solution.prepend(min);
        } else {
            solution.insert(min, min_pos);
        }
    }

    solution.cost +=
        tsp.adj_matrix[solution.path.back()][solution.path.front()];
    return solution;
}

solution_t greedy_cycle(const tsp_t &tsp, unsigned int start) {
    // Select first 3 nodes "optimally"
    solution_t solution(tsp, find_cycle(tsp, start));

    // Select the remaining nodes using GreedyCycle
    while (solution.path.size() < tsp.path_size) {
        std::optional<unsigned int> min;
        int min_cost = INT_MAX;
        int idx = 0;

        for (int i = 0; i < solution.path.size(); i++) {
            auto [min_replace, min_replace_cost] = find_replace(solution, i);

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
