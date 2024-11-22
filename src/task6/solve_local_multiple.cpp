#pragma once

#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task3/solve_local_search.cpp"

solution_t solve_local_multiple_start(const tsp_t &tsp, unsigned total_iters,
                                      unsigned path_size = 100) {
    int best_cost = INT_MAX;
    std::optional<solution_t> best_sol;
    for (unsigned i = 0; i < total_iters; i++) {
        solution_t sol = solve_local_search(gen_random_solution(tsp, path_size),
                                            solution_t::REVERSE, STEEPEST);
        if (!best_sol || sol.cost < best_cost) {
            best_cost = sol.cost;
            best_sol = sol;
        }
    }
    return best_sol.value();
}