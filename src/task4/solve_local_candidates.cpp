#include "../common/types.cpp"
#include "../task1/solve_random.cpp"

#include <algorithm>
#include <chrono>
#include <numeric>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <vector>

typedef std::unordered_map<unsigned int, std::vector<unsigned int>> neighbors_t;

neighbors_t get_nearest_neighbors(const tsp_t &tsp, unsigned k) {
    neighbors_t nn = {};

    for (unsigned int i = 0; i < tsp.n; i++) {
        nn[i] = std::vector<unsigned int>(tsp.n);
        std::iota(nn[i].begin(), nn[i].end(), 0);
        std::vector<int> neighbors(tsp.n);

        std::transform(tsp.adj_matrix[i].begin(), tsp.adj_matrix[i].end(),
                       tsp.weights.begin(), neighbors.begin(),
                       [](int dist, int weight) { return dist + weight; });
        std::stable_sort(
            nn[i].begin(), nn[i].end(),
            [&neighbors](int a, int b) { return neighbors[a] < neighbors[b]; });

        for (std::vector<unsigned int>::iterator it = nn[i].begin();
             it != nn[i].end();) {
            if (*it == i) {
                it = nn[i].erase(it);
            } else {
                ++it;
            }
        }

        nn[i].erase(nn[i].begin() + k, nn[i].end());
    }

    return nn;
}

std::optional<operation_t>
steepest_candidate_search(const solution_t &sol,
                          const neighbors_t &neighbors_map) {
    int delta = 0;
    std::optional<operation_t> best_op;

    for (unsigned node_idx = 0; node_idx < sol.path.size(); node_idx++) {
        unsigned node = sol.path[node_idx];
        for (unsigned neighb : neighbors_map.at(node)) {
            if (sol.remaining_nodes.find(neighb) == sol.remaining_nodes.end()) {
                // Edge Swap (REVERSE) Case
                unsigned neighb_idx =
                    std::find(sol.path.begin(), sol.path.end(), neighb) -
                    sol.path.begin();
                if (neighb_idx == node_idx - 1 || neighb_idx == node_idx + 1)
                    continue;

                unsigned smaller_idx, bigger_idx;
                if (node_idx < neighb_idx) {
                    smaller_idx = node_idx;
                    bigger_idx = neighb_idx;
                } else {
                    smaller_idx = neighb_idx;
                    bigger_idx = node_idx;
                }
                int delta1 =
                    sol.reverse_delta(sol.next(smaller_idx), bigger_idx);
                if (delta1 < delta) {
                    delta = delta1;
                    best_op =
                        operation_t{solution_t::REVERSE, sol.next(smaller_idx),
                                    bigger_idx, delta1};
                }
                int delta2 =
                    sol.reverse_delta(smaller_idx, sol.prev(bigger_idx));
                if (delta2 < delta) {
                    delta = delta2;
                    best_op = operation_t{solution_t::REVERSE, smaller_idx,
                                          sol.prev(bigger_idx), delta2};
                }
            } else {
                // Edge Replace Case (i.e. Node Replace with prev() and next())
                int delta1 = sol.replace_delta(neighb, sol.prev(node_idx));
                if (delta1 < delta) {
                    delta = delta1;
                    best_op = operation_t{solution_t::REPLACE, neighb,
                                          sol.prev(node_idx), delta1};
                }
                int delta2 = sol.replace_delta(neighb, sol.next(node_idx));
                if (delta2 < delta) {
                    delta = delta2;
                    best_op = operation_t{solution_t::REPLACE, neighb,
                                          sol.next(node_idx), delta2};
                }
            }
        }
    }
    return best_op;
}

solution_t local_candidates_steepest(const tsp_t &tsp, solution_t solution,
                                     const neighbors_t &neighbors_map) {
    while (true) {
        std::optional<operation_t> best_op =
            steepest_candidate_search(solution, neighbors_map);

        solution.search_iters++;

        if (!best_op.has_value()) {
            break;
        }

        switch (best_op->type) {
        case solution_t::REVERSE:
            solution.reverse(best_op->arg1, best_op->arg2);
            break;
        case solution_t::REPLACE:
            solution.replace(best_op->arg1, best_op->arg2);
            break;
        default:
            throw std::logic_error("Invalid operation type");
            break;
        }
        if (!solution.is_valid()) {
            throw std::logic_error("Solution is invalid");
        } else if (!solution.is_cost_correct()) {
            throw std::logic_error("Solution cost is incorrect");
        }
    }

    return solution;
}

std::vector<solution_t> solve_local_candidates_steepest_random(const tsp_t &tsp,
                                                               unsigned n) {
    neighbors_t neighbors = get_nearest_neighbors(tsp, 10);
    std::vector<solution_t> solutions = solve_random(tsp, n);
    for (int i = 0; i < solutions.size(); i++) {
        const auto start = std::chrono::high_resolution_clock().now();
        solutions[i] = local_candidates_steepest(tsp, solutions[i], neighbors);
        solutions[i].runtime_ms +=
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock().now() - start)
                .count();
    }
    return solutions;
}
