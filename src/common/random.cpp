#pragma once

#include "types.cpp"

#include <random>

int random_num(int start, int end) {
    std::mt19937 g((std::random_device()()));
    std::uniform_int_distribution<> d(start, end - 1);
    return d(g);
}

template <typename T> int random_num(const std::vector<T> &weights) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<> d(weights.cbegin(), weights.cend());
    return d(gen);
}

solution_t gen_random_solution(const tsp_t &tsp) {
    list_t nodes(tsp.n);
    std::iota(nodes.begin(), nodes.end(), 0);

    std::mt19937 gen((std::random_device()()));
    std::shuffle(nodes.begin(), nodes.end(), gen);
    return solution_t(tsp, {nodes.begin(), nodes.begin() + tsp.path_size});
}
