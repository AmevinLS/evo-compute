#pragma once

#include "../common/types.cpp"

#include <optional>
#include <stdexcept>

std::optional<operation_t> steepest_candidate_search(const solution_t &sol) {
    int delta = 0;
    std::optional<operation_t> best_op;

    for (unsigned node_idx = 0; node_idx < sol.path.size(); node_idx++) {
        unsigned node = sol.path[node_idx];
        for (unsigned neighb : sol.tsp->adj_matrix.neighbors(node, 10)) {
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

solution_t local_candidates(solution_t solution) {
    while (true) {
        std::optional<operation_t> best_op =
            steepest_candidate_search(solution);

        solution.search_iters++;

        if (!best_op.has_value()) {
            break;
        }

        switch (best_op->type) {
        case REVERSE:
            solution.reverse(best_op->arg1, best_op->arg2);
            break;
        case REPLACE:
            solution.replace(best_op->arg1, best_op->arg2);
            break;
        default:
            throw std::logic_error("Invalid operation type");
            break;
        }
        if (!solution.is_valid()) {
            throw std::logic_error("Solution is invalid");
        }
    }

    return solution;
}
