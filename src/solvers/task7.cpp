#pragma once

#include "../common/random.cpp"
#include "../common/types.cpp"
#include "task2.cpp"
#include "task3.cpp"

#include <vector>

solution_t destroy_solution(solution_t sol) {
    int num_nodes_to_remove = sol.path.size() * 0.25;
    std::vector<int> weights = std::vector(sol.path.size(), 1);

    for (int i = 0; i < num_nodes_to_remove; i++) {
        int idx_to_remove = random_num(weights);
        weights.erase(weights.begin() + idx_to_remove);
        sol.remove(idx_to_remove);
    }

    return sol;
}

solution_t large_neighborhood_search(const tsp_t &tsp,
                                     unsigned int time_limit_ms, bool ls) {
    solution_t solution =
        local_search(gen_random_solution(tsp), REVERSE, STEEPEST);
    solution_t best = solution;
    timer_t timer;
    int i = 1;

    timer.start();
    while (timer.measure() < time_limit_ms) {
        solution = destroy_solution(best); // Destroy solution
        solution =
            greedy_cycle_regret(solution, REGRET_WEIGHT); // Repair solution

        if (ls) {
            solution = local_search(solution, REVERSE, STEEPEST);
        }

        if (solution.cost < best.cost) {
            best = solution;
        }

        i++;
    }

    best.search_iters = i;
    return best;
}
