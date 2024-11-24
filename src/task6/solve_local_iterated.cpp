#pragma once

#include <chrono>
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
void perturb_solution(solution_t &sol) {
    // for (int i = 0; i < 3; i++) {
    //     auto iter = sol.remaining_nodes.begin();
    //     std::advance(iter, random_num(0, sol.remaining_nodes.size()));
    //     sol.replace(*iter, random_num(0, sol.path.size()));
    // }
    const unsigned NUM_ALTERATIONS = 10;
    const unsigned MAX_SHIFT = 5;
    const double REVERSE_PROB = 0.6, SWAP_PROB = 0.2;
    for (unsigned i = 0; i < NUM_ALTERATIONS; i++) {
        unsigned idx1 = random_num(0, sol.path.size());
        unsigned shift = random_num(0, MAX_SHIFT + 1);
        unsigned idx2 = idx1;
        for (unsigned j = 0; j < shift; j++)
            idx2 = sol.next(idx2);
        if (idx1 > idx2)
            std::swap(idx1, idx2);

        int prob = random_num(0, 100);
        if (prob < REVERSE_PROB * 100) {
            sol.reverse(idx1, idx2);
        } else if (prob < (REVERSE_PROB + SWAP_PROB) * 100) {
            sol.swap(idx1, idx2);
        } else {
            auto iter = sol.remaining_nodes.begin();
            std::advance(iter, random_num(0, sol.remaining_nodes.size()));
            sol.replace(*iter, random_num(0, sol.path.size()));
        }
    }
}

solution_t solve_local_iterated_search(solution_t solution,
                                       unsigned time_limit_ms) {
    int best_cost = INT_MAX;
    solution_t best_sol = solution;
    solution_t curr_sol = solution;

    const std::chrono::time_point start_time =
        std::chrono::high_resolution_clock::now();
    while (true) {
        curr_sol = solve_local_search(curr_sol, solution_t::REVERSE, STEEPEST);

        const std::chrono::time_point curr_time =
            std::chrono::high_resolution_clock::now();
        int curr_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          curr_time - start_time)
                          .count();
        if (curr_ms > time_limit_ms)
            break;

        if (curr_sol.cost < best_cost) {
            best_cost = curr_sol.cost;
            best_sol = curr_sol;
        }
        perturb_solution(curr_sol);
    }
    return curr_sol;
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
