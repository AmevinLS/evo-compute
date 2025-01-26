#pragma once

#include "print.cpp"
#include "types.cpp"

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

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

std::optional<std::ifstream> open_file(const std::string &fname) {
    std::ifstream in(fname);
    if (!in.is_open()) {
        std::cerr << ERROR << " failed to open file: " << fname << std::endl;
        std::cerr
            << "Check if the file exists and you have permission to read it."
            << std::endl;
        return {};
    }
    return in;
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

std::optional<tsp_t> parse(const std::string &fname) {
    auto in = open_file(fname);

    if (!in.has_value()) {
        return std::nullopt;
    }

    std::vector<node_t> nodes = read(in.value());
    adj_matrix_t matrix = matrixof(nodes);
    return tsp_t(nodes, matrix);
}

std::string instance_name(const std::string &fname) {
    std::string instance_name = fname.substr(fname.find_last_of("/\\") + 1);
    instance_name = instance_name.substr(0, instance_name.find_last_of("."));
    return instance_name;
}
