#pragma once

#include "common/types.cpp"
#include "task1/solve_greedy_cycle.cpp"
#include "task1/solve_nn.cpp"
#include "task1/solve_random.cpp"
#include "task2/solve_greedy_regret.cpp"
#include "task3/solve_local_search.cpp"
#include "task4/solve_local_candidates.cpp"
#include "task5/solve_local_deltas.cpp"
#include "task6/solve_local_iterated.cpp"
#include "task6/solve_local_multiple.cpp"
#include "task7/solve_large_neighbors.cpp"
#include "task9/solve_hybrid_evolutionary.cpp"

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
    LOCAL_CANDIDATES_RANDOM_STEEPEST,
    LOCAL_DELTAS_RANDOM_STEEPEST,
    LOCAL_SEARCH_MULTIPLE_START,
    LOCAL_SEARCH_ITERATED,
    LOCAL_SEARCH_LARGE_NEIGHBOURHOOD_LS,
    LOCAL_SEARCH_LARGE_NEIGHBOURHOOD_NO_LS,
    HYBRID_EVOLUTIONARY_FILL,
    HYBRID_EVOLUTIONARY_REPAIR_NO_LS,
    HYBRID_EVOLUTIONARY_REPAIR_LS
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
    {LOCAL_CANDIDATES_RANDOM_STEEPEST, "local_candidates_random_steepest"},
    {LOCAL_DELTAS_RANDOM_STEEPEST, "local_deltas_random_steepest"},
    {LOCAL_SEARCH_MULTIPLE_START, "local_search_multiple_start"},
    {LOCAL_SEARCH_ITERATED, "local_search_iterated"},
    {LOCAL_SEARCH_LARGE_NEIGHBOURHOOD_LS,
     "local_search_large_neighbourhood_ls"},
    {LOCAL_SEARCH_LARGE_NEIGHBOURHOOD_NO_LS,
     "local_search_large_neighbourhood_no_ls"},
    {HYBRID_EVOLUTIONARY_FILL, "hybrid_evolutionary_fill"},
    {HYBRID_EVOLUTIONARY_REPAIR_NO_LS, "hybrid_evolutionary_repair_no_ls"},
    {HYBRID_EVOLUTIONARY_REPAIR_LS,
     "local_search_hybrid_evolutionary_repair_ls"}};

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
    random_heuristics_to_fn = {
        {RANDOM, solve_random},
        {LOCAL_SEARCH_RANDOM_GREEDY_SWAP,
         solve_local_search_random_greedy_swap},
        {LOCAL_SEARCH_RANDOM_GREEDY_REVERSE,
         solve_local_search_random_greedy_reverse},
        {LOCAL_SEARCH_RANDOM_STEEPEST_SWAP,
         solve_local_search_random_steepest_swap},
        {LOCAL_SEARCH_RANDOM_STEEPEST_REVERSE,
         solve_local_search_random_steepest_reverse},
        {LOCAL_CANDIDATES_RANDOM_STEEPEST,
         solve_local_candidates_steepest_random},
        {LOCAL_DELTAS_RANDOM_STEEPEST, solve_local_deltas_steepest_random},
        {LOCAL_SEARCH_MULTIPLE_START, solve_local_search_multiple_start},
        {LOCAL_SEARCH_ITERATED, solve_local_search_iterated},
        {LOCAL_SEARCH_LARGE_NEIGHBOURHOOD_LS,
         solve_large_neighborhood_search_ls},
        {LOCAL_SEARCH_LARGE_NEIGHBOURHOOD_NO_LS,
         solve_large_neighborhood_search_nols},
        {HYBRID_EVOLUTIONARY_FILL, solve_hybrid_evolutionary_fill},
        {HYBRID_EVOLUTIONARY_REPAIR_NO_LS,
         solve_hybrid_evolutionary_repair_no_ls},
        {HYBRID_EVOLUTIONARY_REPAIR_LS, solve_hybrid_evolutionary_repair_ls}};

std::vector<solution_t> solve(const tsp_t &tsp, heuristic_t heuristic) {
    std::vector<solution_t> solutions;
    unsigned int n = ceil(tsp.n / 2.0);

    if (random_heuristics_to_fn.find(heuristic) !=
        random_heuristics_to_fn.end()) {
        return random_heuristics_to_fn[heuristic](tsp, n);
    }

    const auto fn = gen_heuristics_to_fn[heuristic];
    timer_t timer;

    for (unsigned int i = 0; i < tsp.n; i++) {
        timer.start();
        solution_t solution = fn(tsp, n, i);
        solution.runtime_ms = timer.measure();
        solutions.push_back(solution);
    }

    return solutions;
}
