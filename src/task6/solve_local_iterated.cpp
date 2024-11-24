#pragma once

#include <random>

#include "../common/types.cpp"
#include "../task3/solve_local_search.cpp"

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

solution_t solve_local_iterated_search(const tsp_t &tsp, unsigned int path_size,
                                       unsigned int time_limit_ms) {
    int best_cost = INT_MAX;
    solution_t solution = gen_random_solution(tsp, path_size);
    solution_t best = solution;
    timer_t timer;

    timer.start();
    while (timer.measure() < time_limit_ms) {
        solution = solve_local_search(solution, solution_t::REVERSE, STEEPEST);

        if (solution.cost < best_cost) {
            best_cost = solution.cost;
            best = solution;
        }

        perturb_solution(solution);
    }

    return best;
}

solution_t solve_local_multiple_start(const tsp_t &tsp, unsigned int n) {
    std::vector<solution_t> solutions =
        solve_local_search(tsp, n, solution_t::REVERSE, STEEPEST);
    std::sort(solutions.begin(), solutions.end(),
              [](const solution_t &s1, const solution_t &s2) {
                  return s1.cost < s2.cost;
              });
    return solutions[0];
}
