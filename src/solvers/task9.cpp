#pragma once

#include "../common/evo.cpp"
#include "../common/random.cpp"
#include "../common/search.cpp"
#include "../common/types.cpp"
#include "task2.cpp"
#include "task3.cpp"

#include <iterator>
#include <set>
#include <vector>

#define POP_SIZE 20

// Pick the common edges from the two solutions, fill the rest with random nodes
solution_t random_fill_op(const solution_t &sol1, const solution_t &sol2) {
    return solution_t(*(sol1.tsp), combine(sol1, sol2, true));
};

// Pick the common edges from the two solutions, fix the rest with greedy cycle
// w/ weighted regret
solution_t heuristic_repair_op(const solution_t &sol1, const solution_t &sol2) {
    std::vector<unsigned> new_path = combine(sol1, sol2, false);
    if (new_path.size() == 0) {
        new_path = find_cycle(*(sol1.tsp), random_num(0, 200));
    } else if (new_path.size() < 3) {
        unsigned start_node = new_path[random_num(0, new_path.size())];
        new_path = find_cycle(*(sol1.tsp), start_node);
    }

    solution_t res_sol(*(sol1.tsp), new_path);
    res_sol = greedy_cycle_regret(res_sol, REGRET_WEIGHT);
    return res_sol;
}

solution_t hybrid_evolutionary(const tsp_t &tsp, evo_op_t op,
                               bool ls_after_recomb, unsigned time_limit_ms) {
    std::vector<solution_t> population;
    std::vector<int> pop_weights(POP_SIZE, 1);
    std::unordered_set<unsigned int> pop_costs;
    std::multiset<individual_t> cost_tracker;

    population.reserve(POP_SIZE);
    pop_costs.reserve(POP_SIZE);

    for (unsigned i = 0; i < POP_SIZE; i++) {
        solution_t sol = gen_random_solution(tsp);
        sol = local_search(sol, solution_t::REVERSE, STEEPEST);
        population.push_back(sol);
        pop_costs.insert(sol.cost);
        cost_tracker.insert({sol.cost, i});
    }

    timer_t timer;
    timer.start();
    unsigned search_iters = 0;
    while (timer.measure() < time_limit_ms) {
        // Draw at random two different solutions
        auto [par1, par2] = random_tournament(pop_weights);

        // Construct offspring by recombining parents
        solution_t child = op(population[par1], population[par2]);
        if (ls_after_recomb) {
            child = local_search(child, solution_t::REVERSE, STEEPEST);
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
