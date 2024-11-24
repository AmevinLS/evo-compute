#pragma once

#include <vector>

#include "../common/types.cpp"
#include "../task3/solve_local_search.cpp"

solution_t local_search_multiple_start(const tsp_t &tsp, unsigned int n) {
    std::vector<solution_t> solutions =
        solve_local_search(tsp, n, solution_t::REVERSE, STEEPEST);
    std::sort(solutions.begin(), solutions.end(),
              [](const solution_t &s1, const solution_t &s2) {
                  return s1.cost < s2.cost;
              });
    return solutions[0];
}

std::vector<solution_t> solve_local_search_multiple_start(const tsp_t &tsp,
                                                          unsigned int n) {
    std::vector<solution_t> solutions;
    solutions.reserve(20);
    timer_t timer;
    for (unsigned int i = 0; i < 20; i++) {
        timer.start();
        solutions.push_back(local_search_multiple_start(tsp, n));
        solutions.back().runtime_ms = timer.measure();
    }
    return solutions;
}
