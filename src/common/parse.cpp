#pragma once

#include <algorithm>
#include <cmath>
#include <fstream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

#include "types.cpp"

int l2(const node_t a, const node_t b) {
    return int(round(sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2))));
}

adj_matrix_t matrixof(const std::vector<node_t> nodes) {
    adj_matrix_t matrix(nodes.size());

    for (unsigned int i = 0; i < nodes.size(); i++) {
        for (unsigned int j = 0; j < nodes.size(); j++) {
            if (i == j) {
                continue;
            }

            matrix[i][j] = l2(nodes[i], nodes[j]);
        }

        std::iota(matrix.nn[i].begin(), matrix.nn[i].end(), 0);
        list_t neighbors = matrix[i];

        std::transform(neighbors.cbegin(), neighbors.cend(), nodes.cbegin(),
                       neighbors.begin(), [](int dist, const node_t &node) {
                           return dist + node.weight;
                       });

        neighbors[i] = INT_MAX;

        std::stable_sort(
            matrix.nn[i].begin(), matrix.nn[i].end(),
            [&neighbors](int a, int b) { return neighbors[a] < neighbors[b]; });

        matrix.nn[i].pop_back();
    }

    return matrix;
}

std::vector<node_t> read(std::ifstream &in) {
    std::vector<node_t> nodes;
    std::string line;

    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string value;
        node_t node;

        std::getline(ss, value, ';');
        node.x = std::stoi(value);
        std::getline(ss, value, ';');
        node.y = std::stoi(value);
        std::getline(ss, value, ';');
        node.weight = std::stoi(value);

        nodes.push_back(node);
    }

    return nodes;
}

tsp_t parse(std::ifstream &in) {
    std::vector<node_t> nodes = read(in);
    adj_matrix_t matrix = matrixof(nodes);
    return tsp_t(nodes, matrix);
}
