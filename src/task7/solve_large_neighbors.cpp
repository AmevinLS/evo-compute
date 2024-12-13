#pragma once

#include "../common/random.cpp"
#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task2/solve_greedy_regret.cpp"
#include "../task3/solve_local_search.cpp"
#include "../task6/solve_local_multiple.cpp"

#include <vector>

solution_t destroy_solution(solution_t sol) {
    int num_nodes_to_remove = sol.path.size() * 0.25;
    std::vector<int> weights = std::vector(sol.path.size(), 1);

    // for (int i = 0; i < sol.path.size(); i++) {
    //     weights[i] = -sol.remove_delta(i);
    //     // weights[i] = sol.tsp->weights[sol.path[i]];
    // }

    for (int i = 0; i < num_nodes_to_remove; i++) {
        int idx_to_remove = random_num(weights);
        weights.erase(weights.begin() + idx_to_remove);
        sol.remove(idx_to_remove);
    }

    return sol;
}

solution_t large_neighborhood_search(const tsp_t &tsp, unsigned int path_size,
                                     unsigned int time_limit_ms, bool ls) {
    solution_t solution = solve_local_search(
        gen_random_solution(tsp, path_size), solution_t::REVERSE, STEEPEST);
    solution_t best = solution;
    timer_t timer;
    int i = 1;

    timer.start();
    while (timer.measure() < time_limit_ms) {
        solution = destroy_solution(best); // Destroy solution
        solution =
            solve_regret(solution, path_size, REGRET_WEIGHT); // Repair solution

        if (ls) {
            solution =
                solve_local_search(solution, solution_t::REVERSE, STEEPEST);
        }

        if (solution.cost < best.cost) {
            best = solution;
        }

        i++;
    }

    best.search_iters = i;
    return best;
}

std::vector<solution_t> solve_large_neighborhood_search(const tsp_t &tsp,
                                                        unsigned int path_size,
                                                        bool ls_after_repair) {
    std::vector<solution_t> mslp_solutions =
        solve_local_search_multiple_start(tsp, path_size);
    int time_limit_ms =
        std::accumulate(mslp_solutions.begin(), mslp_solutions.end(), 0,
                        [](int val, const solution_t &sol) {
                            return val + sol.runtime_ms;
                        }) /
        mslp_solutions.size();

    std::vector<solution_t> solutions;
    solutions.reserve(20);
    timer_t timer;
    for (unsigned int i = 0; i < 20; i++) {
        timer.start();
        solutions.push_back(large_neighborhood_search(
            tsp, path_size, time_limit_ms, ls_after_repair));
        solutions.back().runtime_ms = timer.measure();
        solutions.back().search_iters = tsp.n;
    }
    return solutions;
}

std::vector<solution_t>
solve_large_neighborhood_search_ls(const tsp_t &tsp, unsigned int path_size) {
    return solve_large_neighborhood_search(tsp, path_size, true);
}

std::vector<solution_t>
solve_large_neighborhood_search_nols(const tsp_t &tsp, unsigned int path_size) {
    return solve_large_neighborhood_search(tsp, path_size, false);
}
