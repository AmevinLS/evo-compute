#pragma once

#include "../common/random.cpp"
#include "../common/types.cpp"
#include "task3.cpp"

#include <stdexcept>

#define NUM_SOLUTIONS 1000

// Calculates the number of common edges between two solutions
unsigned common_edges_similarity(const solution_t &sol1,
                                 const solution_t &sol2) {

    unsigned num_common_edges = 0;
    for (unsigned i = 0; i < sol1.path.size(); i++) {
        unsigned node1 = sol1.path[i];
        unsigned node2 = sol1.path[sol1.next(i)];
        if (sol2.remaining_nodes.count(node1) +
                sol2.remaining_nodes.count(node2) >
            0)
            continue;

        for (unsigned j = 0; j < sol2.path.size(); j++) {
            unsigned other_node1 = sol2.path[j];
            unsigned other_node2 = sol2.path[sol2.next(j)];
            if ((node1 == other_node1 && node2 == other_node2) ||
                (node1 == other_node2 && node2 == other_node1))
                num_common_edges++;
        }
    }
    return num_common_edges;
}

// Calculates the number of common nodes between two solutions
unsigned common_nodes_similarity(const solution_t &sol1,
                                 const solution_t &sol2) {
    if (sol1.path.size() != sol2.path.size())
        throw std::invalid_argument(
            "Solutions must have paths of the same size");

    unsigned num_common_nodes = 0;
    for (unsigned node : sol1.path) {
        num_common_nodes += (1 - sol2.remaining_nodes.count(node));
    }
    return num_common_nodes;
}

std::vector<similarity_t> gen_similarities(const tsp_t &tsp) {
    unsigned best_sol_idx = 0;

    std::vector<solution_t> sols = {};
    std::vector<similarity_t> sims = {};
    sols.reserve(NUM_SOLUTIONS);
    sols.reserve(NUM_SOLUTIONS);

    for (unsigned i = 0; i < NUM_SOLUTIONS; i++) {
        solution_t sol = gen_random_solution(tsp);
        sol = local_search(sol, solution_t::REVERSE, GREEDY);
        sols.push_back(sol);

        if (sol.cost < sols[best_sol_idx].cost)
            best_sol_idx = i;
    }

    for (unsigned i = 0; i < NUM_SOLUTIONS; i++) {
        double common_edges_sum = 0, common_nodes_sum = 0;
        double denominator = static_cast<double>(sols.size() - 1);

        for (unsigned j = 0; j < sols.size(); j++) {
            if (j == i)
                continue;
            common_edges_sum += common_edges_similarity(sols[i], sols[j]);
            common_nodes_sum += common_nodes_similarity(sols[i], sols[j]);
        }

        sims.push_back(
            {sols[i].cost, common_edges_similarity(sols[i], sols[best_sol_idx]),
             common_nodes_similarity(sols[i], sols[best_sol_idx]),
             common_edges_sum / denominator, common_nodes_sum / denominator});
    }

    return sims;
}
