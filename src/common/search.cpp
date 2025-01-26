#pragma once

#include "types.cpp"

#include <climits>
#include <functional>
#include <optional>
#include <random>
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
                       tsp.adj_matrix[start][i] + tsp.adj_matrix[i][j] +
                       tsp.adj_matrix[j][start];

            if (cost < min_cost || !path.has_value()) {
                min_cost = cost;
                path = {start, i, j};
            }
        }
    }

    return path.value();
}

std::optional<operation_t>
greedy_search(const solution_t &solution,
              std::vector<operation_t> &neighbourhood,
              std::vector<int> &indices, op_type_t op_type) {
    neighbourhood.clear();

    for (unsigned int i = 0; i < solution.path.size(); i++) {
        for (unsigned int j = i + 1; j < solution.path.size(); j++) {
            neighbourhood.emplace_back(operation_t{op_type, i, j, 0});
        }

        for (auto node : solution.remaining_nodes) {
            neighbourhood.emplace_back(operation_t{REPLACE, node, i, 0});
        }
    }

    std::mt19937 g = std::mt19937(std::random_device()());
    std::shuffle(indices.begin(), indices.end(), g);

    for (int idx : indices) {
        operation_t op = neighbourhood[idx];
        int delta = 0;
        switch (op.type) {
        case SWAP:
            delta = solution.swap_delta(op.arg1, op.arg2);
            break;
        case REVERSE:
            delta = solution.reverse_delta(op.arg1, op.arg2);
            break;
        case REPLACE:
            delta = solution.replace_delta(op.arg1, op.arg2);
            break;
        default:
            break;
        }

        if (delta < 0) {
            return op;
        }
    }

    return std::nullopt;
}

std::optional<operation_t> steepest_search(const solution_t &solution,
                                           op_type_t op_type) {
    int delta = 0;
    std::optional<operation_t> best_op;

    for (unsigned int i = 0; i < solution.path.size(); i++) {
        for (unsigned int j = i + 1; j < solution.path.size(); j++) {
            int op_delta = op_type == SWAP ? solution.swap_delta(i, j)
                                           : solution.reverse_delta(i, j);
            if (op_delta < delta) {
                delta = op_delta;
                best_op = operation_t{op_type, i, j, op_delta};
            }
        }

        for (auto node : solution.remaining_nodes) {
            int op_delta = solution.replace_delta(node, i);

            if (op_delta < delta) {
                delta = op_delta;
                best_op = operation_t{REPLACE, node, i, delta};
            }
        }
    }

    return best_op;
}
