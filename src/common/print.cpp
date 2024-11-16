#pragma once

#include "types.cpp"

#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>

std::ostream &operator<<(std::ostream &os, adj_list_t list) {
    int i = 0;
    for (const int &x : list) {
        os << "\t" << i++ << ": " << x << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, adj_matrix_t matrix) {
    int i = 0;
    for (const adj_list_t &row : matrix) {
        os << i++ << ": " << std::endl << row << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, node_t node) {
    os << "(" << node.x << ", " << node.y << ") weight: " << node.weight;
    return os;
}

std::ostream &operator<<(std::ostream &os, std::vector<node_t> nodes) {
    int i = 0;
    for (const node_t &node : nodes) {
        os << i++ << node << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, solution_t solution) {
    os << "Cost: " << solution.cost << "\tRuntime (ms): " << solution.runtime_ms
       << "\tSearch iterations: " << solution.search_iters << std::endl;
    os << "Path: ";

    for (const unsigned int &node : solution.path) {
        os << node << " ";
    }

    os << std::endl;

    return os;
}

std::ostream &operator<<(std::ostream &os, std::vector<solution_t> solutions) {
    int i = 0;
    for (const solution_t &solution : solutions) {
        os << "Solution " << i++ << ":" << std::endl << solution << std::endl;
    }
    return os;
}

std::ofstream &operator<<(std::ofstream &os,
                          std::vector<solution_t> solutions) {
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

std::ofstream &operator<<(std::ofstream &os, const operation_t &oper) {
    os << "{delta=" << oper.delta << ", arg1=" << oper.arg1
       << ", arg2=" << oper.arg2 << ", type=" << oper.type << "}";
    return os;
}