#pragma once

#include "types.cpp"

#include <climits>
#include <functional>
#include <optional>
#include <vector>

node_delta_t find_nn(const solution_t &solution,
                     std::function<int(unsigned int)> cost_func) {
    std::optional<unsigned int> min;
    int min_cost = INT_MAX;

    for (unsigned int node : solution.remaining_nodes) {
        int cost = cost_func(node);

        if (cost < min_cost || !min.has_value()) {
            min = node;
            min_cost = cost;
        }
    }

    return {min.value(), min_cost};
}

node_delta_t find_replace(const solution_t &solution, int pos) {
    std::optional<unsigned int> min;
    int min_cost = INT_MAX;

    for (auto node : solution.remaining_nodes) {
        int cost = solution.insert_delta(node, pos);

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
