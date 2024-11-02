#include "../common/parse.cpp"
#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <queue>
#include <stdexcept>
#include <unordered_map>
#include <utility>
#include <vector>

typedef std::unordered_map<unsigned, std::vector<unsigned>> neighbors_t;

neighbors_t get_nearest_neighbors(const tsp_t &tsp, unsigned k_neighbors) {
    struct CompareSecond {
        bool operator()(const std::pair<unsigned, unsigned> &a,
                        const std::pair<unsigned, unsigned> &b) {
            return a.second < b.second;
        }
    };
    std::map<unsigned,
             std::priority_queue<std::pair<unsigned, unsigned>,
                                 std::deque<std::pair<unsigned, unsigned>>,
                                 CompareSecond>>
        queues;

    for (unsigned node = 0; node < tsp.n; node++) {
        queues[node] = {};
    }

    for (unsigned node1 = 0; node1 < tsp.n; node1++) {
        for (unsigned node2 = node1 + 1; node2 < tsp.n; node2++) {
            unsigned distance = tsp.adj_matrix(node1, node2);
            queues[node1].push({node2, (distance + tsp.weights[node2])});
            queues[node2].push({node1, (distance + tsp.weights[node1])});
        }
    }

    neighbors_t neighbors;
    for (unsigned node = 0; node < tsp.n; node++) {
        neighbors[node] = {};
        for (unsigned i = 0; i < k_neighbors; i++) {
            auto [neighb, cost] = queues[node].top();
            queues[node].pop();
            neighbors[node].push_back(neighb);
        }
    }
    return neighbors;
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
    std::vector<int> indices;

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
    auto neighbors_map = get_nearest_neighbors(tsp, 10);
    std::vector solutions = solve_random(tsp, n);
    for (auto &sol : solutions) {
        std::cout << "orig_cost: " << sol.cost << ", ";
        sol = local_candidates_steepest(tsp, sol, neighbors_map);
        std::cout << "final_cost: " << sol.cost
                  << ", num_iters: " << sol.search_iters << "\n";
    }
    return solutions;
}

// int main() {
//     std::ifstream fin("../../data/TSPA.csv");
//     tsp_t tsp = parse(fin);
//     auto solutions = solve_local_candidates_steepest_random(tsp, 100);
//     return 0;
// }