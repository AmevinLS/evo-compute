#pragma once

#include "../common/types.cpp"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <random>
#include <set>
#include <vector>

std::vector<solution_t> solve_random(const tsp_t &tsp, unsigned int path_size) {
    std::vector<unsigned int> indices(tsp.n);
    std::iota(indices.begin(), indices.end(), 0);
    std::set<std::vector<unsigned int>> seen;
    std::vector<solution_t> solutions;

    std::random_device rd;
    std::mt19937 g(rd());
    while (solutions.size() < tsp.n) {
        const auto start = std::chrono::high_resolution_clock().now();
        std::shuffle(indices.begin(), indices.end(), g);
        std::vector<unsigned int> path(indices.begin(),
                                       indices.begin() + path_size);

        if (seen.find(path) != seen.end()) {
            continue;
        }
        solutions.push_back(
            solution_t(tsp, path,
                       std::chrono::duration_cast<std::chrono::milliseconds>(
                           std::chrono::high_resolution_clock().now() - start)
                           .count()));
        seen.insert(path);
    }

    return solutions;
}
