#pragma once

#include "../common/types.cpp"

#include <algorithm>
#include <numeric>
#include <random>
#include <set>
#include <vector>

std::vector<solution_t> solve_random(const tsp_t &tsp, unsigned int n) {
    std::vector<unsigned int> indices(tsp.n);
    std::iota(indices.begin(), indices.end(), 0);
    std::set<std::vector<unsigned int>> seen;
    std::vector<solution_t> solutions;

    std::random_device rd;
    std::mt19937 g(rd());
    while (solutions.size() < tsp.n) {
        std::shuffle(indices.begin(), indices.end(), g);
        std::vector<unsigned int> path(indices.begin(), indices.begin() + n);

        if (seen.find(path) != seen.end()) {
            continue;
        }

        solutions.push_back(solution_t(tsp, path));
        seen.insert(path);
    }

    return solutions;
}
