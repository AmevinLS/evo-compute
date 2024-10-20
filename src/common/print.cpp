#include "types.cpp"

#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>

std::ostream &operator<<(std::ostream &os, adj_list_t list) {
    int i = 1;
    for (const int &x : list) {
        os << "\t" << i++ << ": " << x << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, adj_matrix_t matrix) {
    int i = 1;
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
    int i = 1;
    for (const node_t &node : nodes) {
        os << i++ << node << std::endl;
    }
    return os;
}

std::ostream &operator<<(std::ostream &os, solution_t solution) {
    os << "Cost: " << solution.cost << std::endl;
    os << "Path: ";

    for (const unsigned int &node : solution.path) {
        os << node + 1 << " ";
    }

    os << std::endl;

    return os;
}

std::ostream &operator<<(std::ostream &os, std::vector<solution_t> solutions) {
    int i = 1;
    for (const solution_t &solution : solutions) {
        os << "Solution " << i++ << ":" << std::endl << solution << std::endl;
    }
    return os;
}

std::ofstream &operator<<(std::ofstream &os,
                          std::vector<solution_t> solutions) {
    os << "idx,cost,path" << std::endl;
    int i = 1;

    for (const solution_t &solution : solutions) {
        os << i++ << "," << solution.cost << ",";

        for (const unsigned int &node : solution.path) {
            os << node << " ";
        }

        os << std::endl;
    }

    return os;
}
