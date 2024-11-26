#pragma once

#include <random>
#include <vector>

#include "../common/types.cpp"
#include "../task3/solve_local_search.cpp"
#include "solve_local_multiple.cpp"

int random_num(int start, int end) {
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(start, end - 1); // define the range
    return distr(gen);
}

// Perturb the solution in-place
void perturb_solution(solution_t &solution, unsigned int alterations = 10,
                      unsigned int max_shift = 15, double reverse_prob = 40,
                      double swap_prob = 80) {
    for (unsigned int i = 0; i < alterations; i++) {
        unsigned int idx1 = random_num(0, solution.path.size());
        unsigned int shift = random_num(0, max_shift + 1);
        int prob = random_num(0, 100);
        unsigned idx2 = idx1;

        for (unsigned j = 0; j < shift; j++)
            idx2 = solution.next(idx2);

        if (idx1 > idx2)
            std::swap(idx1, idx2);

        if (prob < reverse_prob) {
            solution.reverse(idx1, idx2);
        } else if (prob < swap_prob) {
            solution.swap(idx1, idx2);
        } else {
            auto iter = solution.remaining_nodes.begin();
            std::advance(iter, random_num(0, solution.remaining_nodes.size()));
            solution.replace(*iter, random_num(0, solution.path.size()));
        }
    }
}

solution_t local_search_iterated(const tsp_t &tsp, unsigned int path_size,
                                 unsigned int time_limit_ms) {
    int best_cost = INT_MAX;
    solution_t solution = gen_random_solution(tsp, path_size);
    solution_t best = solution;
    timer_t timer;

    timer.start();
    int i = 1;
    while (timer.measure() < time_limit_ms) {
        solution = solve_local_search(solution, solution_t::REVERSE, STEEPEST);

        if (solution.cost < best_cost) {
            best_cost = solution.cost;
            best = solution;
        }

        perturb_solution(solution);
        i++;
    }

    best.search_iters = i;
    return best;
}

std::vector<solution_t> solve_local_search_iterated(const tsp_t &tsp,
                                                    unsigned int path_size) {
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
        solutions.push_back(
            local_search_iterated(tsp, path_size, time_limit_ms));
        solutions.back().runtime_ms = timer.measure();
    }

    return solutions;
}
