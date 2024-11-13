#pragma once

#include "common/types.cpp"
#include "task1/solve_greedy_cycle.cpp"
#include "task1/solve_nn.cpp"
#include "task1/solve_random.cpp"
#include "task2/solve_greedy_regret.cpp"
#include "task3/solve_local_search.cpp"
#include "task4/solve_local_candidates.cpp"

#include <chrono>
#include <map>
#include <vector>

enum heuristic_t {
    RANDOM,
    NN_END,
    NN_ANY,
    GREEDY_CYCLE,
    GREEDY_CYCLE_REGRET,
    GREEDY_CYCLE_REGRET_WEIGHTED,
    LOCAL_SEARCH_GEN_GREEDY_SWAP,
    LOCAL_SEARCH_GEN_GREEDY_REVERSE,
    LOCAL_SEARCH_GEN_STEEPEST_SWAP,
    LOCAL_SEARCH_GEN_STEEPEST_REVERSE,
    LOCAL_SEARCH_RANDOM_GREEDY_SWAP,
    LOCAL_SEARCH_RANDOM_GREEDY_REVERSE,
    LOCAL_SEARCH_RANDOM_STEEPEST_SWAP,
    LOCAL_SEARCH_RANDOM_STEEPEST_REVERSE,
    LOCAL_CANDIDATES_RANDOM_STEEPEST
};

std::map<heuristic_t, std::string> heuristic_t_str = {
    {RANDOM, "random"},
    {NN_END, "nn_end"},
    {NN_ANY, "nn_any"},
    {GREEDY_CYCLE, "greedy_cycle"},
    {GREEDY_CYCLE_REGRET, "greedy_cycle_regret"},
    {GREEDY_CYCLE_REGRET_WEIGHTED, "greedy_cycle_regret_weighted"},
    {LOCAL_SEARCH_GEN_GREEDY_SWAP, "local_search_gen_greedy_swap"},
    {LOCAL_SEARCH_GEN_GREEDY_REVERSE, "local_search_gen_greedy_reverse"},
    {LOCAL_SEARCH_GEN_STEEPEST_SWAP, "local_search_gen_steepest_swap"},
    {LOCAL_SEARCH_GEN_STEEPEST_REVERSE, "local_search_gen_steepest_reverse"},
    {LOCAL_SEARCH_RANDOM_GREEDY_SWAP, "local_search_random_greedy_swap"},
    {LOCAL_SEARCH_RANDOM_GREEDY_REVERSE, "local_search_random_greedy_reverse"},
    {LOCAL_SEARCH_RANDOM_STEEPEST_SWAP, "local_search_random_steepest_swap"},
    {LOCAL_SEARCH_RANDOM_STEEPEST_REVERSE,
     "local_search_random_steepest_reverse"},
    {LOCAL_CANDIDATES_RANDOM_STEEPEST, "local_candidates_random_steepest"}};

std::map<heuristic_t, solution_t (*)(const tsp_t &, unsigned int, unsigned int)>
    gen_heuristics_to_fn = {
        {NN_END, solve_nn_end},
        {NN_ANY, solve_nn_any},
        {GREEDY_CYCLE, solve_greedy_cycle},
        {GREEDY_CYCLE_REGRET, solve_regret_unweighted},
        {GREEDY_CYCLE_REGRET_WEIGHTED, solve_regret_weighted},
        {LOCAL_SEARCH_GEN_GREEDY_SWAP, solve_local_search_gen_greedy_swap},
        {LOCAL_SEARCH_GEN_GREEDY_REVERSE,
         solve_local_search_gen_greedy_reverse},
        {LOCAL_SEARCH_GEN_STEEPEST_SWAP, solve_local_search_gen_steepest_swap},
        {LOCAL_SEARCH_GEN_STEEPEST_REVERSE,
         solve_local_search_gen_steepest_reverse}};

std::map<heuristic_t, std::vector<solution_t> (*)(const tsp_t &, unsigned int)>
    random_heuristics_to_fn = {{RANDOM, solve_random},
                               {LOCAL_SEARCH_RANDOM_GREEDY_SWAP,
                                solve_local_search_random_greedy_swap},
                               {LOCAL_SEARCH_RANDOM_GREEDY_REVERSE,
                                solve_local_search_random_greedy_reverse},
                               {LOCAL_SEARCH_RANDOM_STEEPEST_SWAP,
                                solve_local_search_random_steepest_swap},
                               {LOCAL_SEARCH_RANDOM_STEEPEST_REVERSE,
                                solve_local_search_random_steepest_reverse},
                               {LOCAL_CANDIDATES_RANDOM_STEEPEST,
                                solve_local_candidates_steepest_random}};

std::vector<solution_t> solve(const tsp_t &tsp, heuristic_t heuristic) {
    std::vector<solution_t> solutions;
    unsigned int n = ceil(tsp.n / 2.0);

    if (random_heuristics_to_fn.find(heuristic) !=
        random_heuristics_to_fn.end()) {
        return random_heuristics_to_fn[heuristic](tsp, n);
    }

    const auto fn = gen_heuristics_to_fn[heuristic];

    for (unsigned int i = 0; i < tsp.n; i++) {
        const auto start = std::chrono::high_resolution_clock::now();
        solution_t solution = fn(tsp, n, i);
        solution.runtime_ms =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now() - start)
                .count();
        solutions.push_back(solution);
    }

    return solutions;
}
