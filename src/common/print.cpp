#pragma once

#include "types.cpp"

#include <fstream>
#include <iostream>
#include <optional>
#include <ostream>
#include <sstream>
#include <vector>

std::ostream &operator<<(std::ostream &os, const adj_matrix_t matrix) {
    int i = 0;
    for (const auto &row : matrix) {
        os << i++ << ": " << std::endl << "\t";

        for (const int &x : row) {
            os << x << ", ";
        }

        os << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const node_t node) {
    os << "\t(" << node.x << ", " << node.y << ") weight: " << node.weight;
    return os;
}

std::ostream &operator<<(std::ostream &os, const std::vector<node_t> nodes) {
    int i = 0;
    for (const node_t &node : nodes) {
        os << i++ << node << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, const tsp_t tsp) {
    os << "Nodes: " << std::endl << tsp.nodes << std::endl;
    os << "Adjacency matrix: " << std::endl << tsp.adj_matrix << std::endl;
    return os;
}

std::ostream &operator<<(std::ostream &os, solution_t solution) {
    os << "Cost: " << solution.cost << "\tRuntime (ms): " << solution.runtime_ms
       << "\tSearch iterations: " << solution.search_iters << std::endl;
    os << "Path: ";

    for (const unsigned int &node : solution.path) {
        os << node << ", ";
    }

    os << std::endl;

    return os;
}

std::ostream &operator<<(std::ostream &os,
                         const std::vector<solution_t> solutions) {
    os << "Found " << solutions.size() << " solutions" << std::endl;

    if (solutions.empty()) {
        return os;
    }

    os << std::endl << "Solutions:" << std::endl;

    int i = 0;
    for (const solution_t &solution : solutions) {
        os << "Solution " << i++ << ":" << std::endl << solution << std::endl;
    }

    os << std::endl;

    std::optional<solution_t> best;
    int min_cost = INT_MAX, min_time = INT_MAX, max_cost = 0, max_time = 0;
    double avg_cost = 0, avg_time = 0;

    for (const solution_t &solution : solutions) {
        avg_cost += solution.cost;
        avg_time += solution.runtime_ms;
        if (solution.cost < min_cost) {
            min_cost = solution.cost;
            best = solution;
        }
        if (solution.cost > max_cost) {
            max_cost = solution.cost;
        }
        if (solution.runtime_ms < min_time) {
            min_time = solution.runtime_ms;
        }
        if (solution.runtime_ms > max_time) {
            max_time = solution.runtime_ms;
        }
    }

    os << "Stats:" << std::endl;
    os << "Cost: " << min_cost << " / " << avg_cost / solutions.size() << " / "
       << max_cost << std::endl;
    os << "Runtime (ms): " << min_time << " / " << avg_time / solutions.size()
       << " / " << max_time << std::endl;
    os << std::endl;

    if (best.has_value()) {
        os << "Best solution:" << std::endl << best.value() << std::endl;
    }

    return os;
}

std::ofstream &operator<<(std::ofstream &os,
                          const std::vector<solution_t> solutions) {
    os << "idx,cost,runtime_ms,search_iters,path" << std::endl;
    int i = 0;

    for (const solution_t &solution : solutions) {
        os << i++ << "," << solution.cost << "," << solution.runtime_ms << ","
           << solution.search_iters << ",";

        for (const unsigned int &node : solution.path) {
            os << node << " ";
        }

        os << std::endl;
    }

    return os;
}

std::ofstream &operator<<(std::ofstream &os, const operation_t &op) {
    os << "{delta=" << op.delta << ", arg1=" << op.arg1 << ", arg2=" << op.arg2
       << ", type=" << op.type << "}";
    return os;
}

template <class T> std::string VecToString(const std::vector<T> &v) {
    std::stringstream ss;
    ss << "{ ";
    for (unsigned i = 0; i < v.size(); i++) {
        ss << v[i] << " ";
    }
    ss << "}";
    return ss.str();
}