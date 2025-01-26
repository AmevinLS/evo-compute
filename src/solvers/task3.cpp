#pragma once

#include "../common/search.cpp"
#include "../common/types.cpp"

#include <optional>
#include <vector>

solution_t local_search(solution_t solution, op_type_t op_type,
                        search_t search_type) {
    unsigned int path_size = solution.path.size();
    unsigned int neighbourhood_size =
        path_size * (path_size - 1) / 2 +
        path_size * solution.remaining_nodes.size();
    std::vector<operation_t> neighbourhood;
    std::vector<int> indices;

    if (search_type == GREEDY) {
        neighbourhood.reserve(neighbourhood_size);
        indices.reserve(neighbourhood_size);

        for (int i = 0; i < neighbourhood_size; i++) {
            indices.push_back(i);
        }
    }

    while (true) {
        std::optional<operation_t> best_op =
            search_type == GREEDY
                ? greedy_search(solution, neighbourhood, indices, op_type)
                : steepest_search(solution, op_type);

        solution.search_iters++;

        if (!best_op.has_value()) {
            break;
        }

        switch (best_op->type) {
        case SWAP:
            solution.swap(best_op->arg1, best_op->arg2);
            break;
        case REVERSE:
            solution.reverse(best_op->arg1, best_op->arg2);
            break;
        case REPLACE:
            solution.replace(best_op->arg1, best_op->arg2);
            break;
        default:
            break;
        }
    }

    return solution;
}
