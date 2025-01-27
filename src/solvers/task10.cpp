#pragma once

#include "../common/print.cpp"
#include "../common/random.cpp"
#include "../common/types.cpp"
#include "task2.cpp"
#include "task3.cpp"

#include <cmath>
#include <iostream>

enum destroy_t { kUniform, kWeighted };

solution_t destroy_solution(const tsp_t &tsp, solution_t sol,
                            destroy_t node_select) {
    int num_nodes_to_remove = sol.path.size() * 0.25;

    std::vector<int> weights;
    if (node_select == kWeighted) {
        weights = std::vector<int>(sol.path.size());
        for (int i = 0; i < sol.path.size(); i++) {
            weights[i] = tsp.weights[sol.path[i]];
        }
    } else if (node_select == kUniform) {
        weights = std::vector<int>(sol.path.size(), 1);
    }

    for (int i = 0; i < num_nodes_to_remove; i++) {
        int idx_to_remove = random_num(weights);
        weights.erase(weights.begin() + idx_to_remove);
        sol.remove(idx_to_remove);
    }

    return sol;
}

void normalize(std::vector<double> &distrib) {
    double total = 0;
    for (double prob : distrib) {
        total += prob;
    }
    for (unsigned i = 0; i < distrib.size(); i++) {
        distrib[i] /= total;
    }
}

solution_t custom_lns(const tsp_t &tsp, unsigned int time_limit_ms) {
    solution_t solution =
        local_search(gen_random_solution(tsp), REVERSE, STEEPEST);
    solution_t best = solution;
    timer_t timer;
    int i = 1;

    const std::vector<destroy_t> node_selects = {destroy_t::kUniform,
                                                 destroy_t::kWeighted};
    std::vector<double> node_select_probs = {1, 1};

    const std::vector<double> regret_weights = {0.1, 0.2, 0.25, 0.3, 0.4};
    std::vector<double> regret_weight_probs = {1, 1, 1, 1, 1};

    timer.start();
    while (timer.measure() < time_limit_ms) {
        std::cout << "Iteration " << i << std::endl;

        unsigned node_select_idx = random_num(node_select_probs);
        solution = destroy_solution(
            tsp, best, node_selects[node_select_idx]); // Destroy solution

        unsigned regret_weight_idx = random_num(regret_weight_probs);
        solution = greedy_cycle_regret(
            solution,
            regret_weights[regret_weight_idx]); // Repair solution

        solution = local_search(solution, REVERSE, STEEPEST);

        double delta = solution.cost - best.cost;
        const double update_coeff = -0.005;
        if (delta < 0) {
            best = solution;
        }
        node_select_probs[node_select_idx] *= exp(update_coeff * delta);
        normalize(node_select_probs);
        regret_weight_probs[regret_weight_idx] *= exp(update_coeff * delta);
        normalize(regret_weight_probs);

        i++;
    }

    std::cout << "Final node_select_probs: " << node_select_probs << std::endl;
    std::cout << "Final regret_weight_probs: " << regret_weight_probs
              << std::endl;

    best.search_iters = i;
    return best;
}
