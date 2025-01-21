#pragma once

#include "../common/random.cpp"
#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task3/solve_local_search.cpp"
#include "../task6/solve_local_multiple.cpp"
#include "recombination_opers.cpp"

#include <functional>
#include <iterator>
#include <set>
#include <utility>
#include <vector>

std::pair<unsigned, unsigned> select_parents(std::vector<int> &weights) {
    unsigned par1 = random_num(weights);
    weights[par1] = 0;
    unsigned par2 = random_num(weights);
    weights[par1] = 1;
    return {par1, par2};
}

struct individual_t {
    int cost;
    unsigned int idx;
};

template <> struct std::less<individual_t> {
    constexpr bool operator()(const individual_t &a,
                              const individual_t &b) const {
        return a.cost > b.cost;
    }
};

solution_t solve_hybrid_evolutionary(
    const tsp_t &tsp, unsigned path_size, unsigned pop_size,
    solution_t (*recomb_oper)(const solution_t &, const solution_t &),
    bool ls_after_recomb, unsigned time_limit_ms) {
    std::vector<solution_t> population;
    std::vector<int> pop_weights(pop_size, 1);
    std::unordered_set<unsigned int> pop_costs;
    std::multiset<individual_t> cost_tracker;

    population.reserve(pop_size);
    pop_costs.reserve(pop_size);

    for (unsigned i = 0; i < pop_size; i++) {
        solution_t sol = gen_random_solution(tsp, path_size);
        sol = solve_local_search(sol, solution_t::REVERSE, STEEPEST);
        population.push_back(sol);
        pop_costs.insert(sol.cost);
        cost_tracker.insert({sol.cost, i});
    }

    timer_t timer;
    timer.start();
    unsigned search_iters = 0;
    while (timer.measure() < time_limit_ms) {
        // Draw at random two different solutions
        auto [par1, par2] = select_parents(pop_weights);

        // Construct offspring by recombining parents
        solution_t child = recomb_oper(population[par1], population[par2]);
        if (ls_after_recomb) {
            child = solve_local_search(child, solution_t::REVERSE, STEEPEST);
        }

        // Replace worst solution in population if better
        auto [worst_cost, worst_idx] = *cost_tracker.cbegin();

        if (child.cost < worst_cost &&
            pop_costs.find(child.cost) == pop_costs.end()) {
            population[worst_idx] = child;
            pop_costs.erase(worst_cost);
            pop_costs.insert(child.cost);
            cost_tracker.erase(*cost_tracker.begin());
            cost_tracker.insert({child.cost, worst_idx});
        }

        search_iters++;
    }

    solution_t res_sol = population[(*cost_tracker.rbegin()).idx];
    res_sol.search_iters = search_iters;
    return res_sol;
}

std::vector<solution_t> solve_hybrid_evolutionary(
    const tsp_t &tsp, unsigned int path_size,
    solution_t (*recomb_oper)(const solution_t &, const solution_t &),
    bool ls_after_recomb) {
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
    for (unsigned i = 0; i < 20; i++) {
        timer.start();
        solutions.push_back(solve_hybrid_evolutionary(
            tsp, path_size, 20, recomb_oper, ls_after_recomb, time_limit_ms));
        solutions.back().runtime_ms = timer.measure();
    }
    return solutions;
}

std::vector<solution_t> solve_hybrid_evolutionary_fill(const tsp_t &tsp,
                                                       unsigned int path_size) {
    return solve_hybrid_evolutionary(tsp, path_size, random_fill_op, false);
}

std::vector<solution_t>
solve_hybrid_evolutionary_repair_no_ls(const tsp_t &tsp,
                                       unsigned int path_size) {
    return solve_hybrid_evolutionary(tsp, path_size, heuristic_repair_op,
                                     false);
}

std::vector<solution_t>
solve_hybrid_evolutionary_repair_ls(const tsp_t &tsp, unsigned int path_size) {
    return solve_hybrid_evolutionary(tsp, path_size, heuristic_repair_op, true);
}
