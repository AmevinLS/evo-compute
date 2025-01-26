#pragma once

#include <vector>

#include "../common/random.cpp"
#include "../common/types.cpp"
#include "task1.cpp"
#include "task3.cpp"

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

solution_t local_search_iterated(const tsp_t &tsp, unsigned int time_limit_ms) {
    int best_cost = INT_MAX;
    solution_t solution = gen_random_solution(tsp);
    solution_t best = solution;
    timer_t timer;

    timer.start();
    int i = 1;
    while (timer.measure() < time_limit_ms) {
        solution = local_search(solution, REVERSE, STEEPEST);

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

solution_t local_search_multiple_start(const tsp_t &tsp) {
    std::vector<solution_t> solutions = random(tsp);

    for (unsigned int i = 0; i < solutions.size(); i++) {
        solutions[i] = local_search(solutions[i], REVERSE, STEEPEST);
    }

    std::sort(solutions.begin(), solutions.end(),
              [](const solution_t &s1, const solution_t &s2) {
                  return s1.cost < s2.cost;
              });
    return solutions[0];
}

unsigned int calc_time_limit_ms(const tsp_t &tsp) {
    int num_runs = 10;
    std::vector<solution_t> solutions;
    solutions.reserve(num_runs);
    timer_t timer;

    for (int i = 0; i < num_runs; i++) {
        timer.start();
        solution_t solution = local_search_multiple_start(tsp);
        solution.runtime_ms = timer.measure();
        solutions.push_back(solution);
    }

    return std::accumulate(solutions.cbegin(), solutions.cend(), 0,
                           [](int val, const solution_t &sol) {
                               return val + sol.runtime_ms;
                           }) /
           num_runs;
}
