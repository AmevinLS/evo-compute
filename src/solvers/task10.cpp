#pragma once

#include "../common/types.cpp"

#include <cmath>
#include <iostream>

enum class DestroyNodeSelect { kUniform, kWeighted };

solution_t CustomDestroySolution(const tsp_t &tsp, solution_t sol,
                                 DestroyNodeSelect node_select) {
    int num_nodes_to_remove = sol.path.size() * 0.25;

    std::vector<int> weights;
    if (node_select == DestroyNodeSelect::kWeighted) {
        weights = std::vector<int>(sol.path.size());
        for (int i = 0; i < sol.path.size(); i++) {
            weights[i] = tsp.weights[sol.path[i]];
        }
    } else if (node_select == DestroyNodeSelect::kUniform) {
        weights = std::vector<int>(sol.path.size(), 1);
    }

    for (int i = 0; i < num_nodes_to_remove; i++) {
        int idx_to_remove = random_num(weights);
        weights.erase(weights.begin() + idx_to_remove);
        sol.remove(idx_to_remove);
    }

    return sol;
}

void NormalizeDistribution(std::vector<double> &distrib) {
    double total = 0;
    for (double prob : distrib) {
        total += prob;
    }
    for (unsigned i = 0; i < distrib.size(); i++) {
        distrib[i] /= total;
    }
}

solution_t CustomLNS(const tsp_t &tsp, unsigned int path_size,
                     unsigned int time_limit_ms, bool ls) {
    solution_t solution = solve_local_search(
        gen_random_solution(tsp, path_size), solution_t::REVERSE, STEEPEST);
    solution_t best = solution;
    timer_t timer;
    int i = 1;

    const std::vector<DestroyNodeSelect> node_selects = {
        DestroyNodeSelect::kUniform, DestroyNodeSelect::kWeighted};
    std::vector<double> node_select_probs = {1, 1};

    const std::vector<double> regret_weights = {0.1, 0.2, 0.25, 0.3, 0.4};
    std::vector<double> regret_weight_probs = {1, 1, 1, 1, 1};

    timer.start();
    while (timer.measure() < time_limit_ms) {
        unsigned node_select_idx = random_num(node_select_probs);
        solution = CustomDestroySolution(
            tsp, best, node_selects[node_select_idx]); // Destroy solution

        unsigned regret_weight_idx = random_num(regret_weight_probs);
        solution =
            solve_regret(solution, path_size,
                         regret_weights[regret_weight_idx]); // Repair solution

        if (ls) {
            solution =
                solve_local_search(solution, solution_t::REVERSE, STEEPEST);
        }

        double delta = solution.cost - best.cost;
        const double update_coeff = -0.005;
        if (delta < 0) {
            best = solution;
        }
        node_select_probs[node_select_idx] *= exp(update_coeff * delta);
        NormalizeDistribution(node_select_probs);
        regret_weight_probs[regret_weight_idx] *= exp(update_coeff * delta);
        NormalizeDistribution(regret_weight_probs);

        i++;
    }

    std::cout << "Final node_select_probs: " << VecToString(node_select_probs)
              << std::endl;
    std::cout << "Final regret_weight_probs: "
              << VecToString(regret_weight_probs) << std::endl;

    best.search_iters = i;
    return best;
}

class CustomAlgo : public Algorithm {
  public:
    solution_t Run(const tsp_t &tsp, unsigned path_size,
                   unsigned time_limit_ms) const {
        return CustomLNS(tsp, path_size, time_limit_ms, true);
    }

    std::string GetName() { return "CustomAlgo"; }
};
