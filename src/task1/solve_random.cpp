#pragma once

#include "../common/types.cpp"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <random>
#include <set>
#include <vector>

int random_num(int start, int end) {
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(start, end - 1); // define the range
    return distr(gen);
}

solution_t gen_random_solution(const tsp_t &tsp, unsigned path_size) {
    std::vector<unsigned> nodes(tsp.n);
    std::iota(nodes.begin(), nodes.end(), 0);

    std::mt19937 gen((std::random_device()()));
    std::shuffle(nodes.begin(), nodes.end(), gen);
    return solution_t(
        tsp, std::vector<unsigned>(nodes.begin(), nodes.begin() + path_size));
}

std::vector<solution_t> solve_random(const tsp_t &tsp, unsigned int path_size) {
    std::vector<unsigned int> indices(tsp.n);
    std::iota(indices.begin(), indices.end(), 0);
    std::set<std::vector<unsigned int>> seen;
    std::vector<solution_t> solutions;

    std::mt19937 g((std::random_device()()));
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
