#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task2/solve_greedy_regret.cpp"

#include <algorithm>
#include <chrono>
#include <optional>
#include <random>
#include <vector>

std::mt19937 g = std::mt19937(std::random_device()());

enum search_t { GREEDY, STEEPEST };

std::optional<operation_t>
greedy_search(const solution_t &solution,
              std::vector<operation_t> &neighbourhood,
              solution_t::op_type_t op_type) {
    neighbourhood.clear();

    for (unsigned int i = 0; i < solution.path.size(); i++) {
        for (unsigned int j = i + 1; j < solution.path.size(); j++) {
            int op_delta = op_type == solution_t::SWAP
                               ? solution.swap_delta(i, j)
                               : solution.reverse_delta(i, j);

            neighbourhood.emplace_back(operation_t{op_type, i, j, op_delta});
        }

        for (auto node : solution.remaining_nodes) {
            int op_delta = solution.replace_delta(node, i);

            neighbourhood.emplace_back(
                operation_t{solution_t::REPLACE, node, i, op_delta});
        }
    }

    std::shuffle(neighbourhood.begin(), neighbourhood.end(), g);

    for (operation_t operation : neighbourhood) {
        if (operation.delta < 0) {
            return operation;
        }
    }

    return std::nullopt;
}

std::optional<operation_t> steepest_search(const solution_t &solution,
                                           solution_t::op_type_t op_type) {
    int delta = 0;
    std::optional<operation_t> best_op;

    for (unsigned int i = 0; i < solution.path.size(); i++) {
        for (unsigned int j = i + 1; j < solution.path.size(); j++) {
            int op_delta = op_type == solution_t::SWAP
                               ? solution.swap_delta(i, j)
                               : solution.reverse_delta(i, j);
            if (op_delta < delta) {
                delta = op_delta;
                best_op = operation_t{op_type, i, j, op_delta};
            }
        }

        for (auto node : solution.remaining_nodes) {
            int op_delta = solution.replace_delta(node, i);

            if (op_delta < delta) {
                delta = op_delta;
                best_op = operation_t{solution_t::REPLACE, node, i, delta};
            }
        }
    }

    return best_op;
}

solution_t solve_local_search(solution_t solution,
                              solution_t::op_type_t op_type,
                              search_t search_type) {
    std::vector<operation_t> neighbourhood;

    if (search_type == GREEDY) {
        unsigned int path_size = solution.path.size();
        neighbourhood.reserve(path_size * (path_size - 1) / 2 +
                              path_size * solution.remaining_nodes.size());
    }

    while (true) {
        std::optional<operation_t> best_op =
            search_type == GREEDY
                ? greedy_search(solution, neighbourhood, op_type)
                : steepest_search(solution, op_type);

        solution.search_iters++;

        if (!best_op.has_value()) {
            break;
        }

        switch (best_op->type) {
        case solution_t::SWAP:
            solution.swap(best_op->arg1, best_op->arg2);
            break;
        case solution_t::REVERSE:
            solution.reverse(best_op->arg1, best_op->arg2);
            break;
        case solution_t::REPLACE:
            solution.replace(best_op->arg1, best_op->arg2);
            break;
        default:
            break;
        }
    }

    return solution;
}

solution_t solve_local_search(const tsp_t &tsp, unsigned int n,
                              unsigned int start, solution_t::op_type_t op_type,
                              search_t search_type) {
    solution_t solution = solve_regret_weighted(tsp, n, start);
    return solve_local_search(solution, op_type, search_type);
}

std::vector<solution_t> solve_local_search(const tsp_t &tsp, unsigned int n,
                                           solution_t::op_type_t op_type,
                                           search_t search_type) {
    std::vector<solution_t> solutions = solve_random(tsp, n);

    for (unsigned int i = 0; i < solutions.size(); i++) {
        const auto start = std::chrono::high_resolution_clock().now();
        int old_runtime_ms = solutions[i].runtime_ms;
        solutions[i] = solve_local_search(solutions[i], op_type, search_type);
        solutions[i].runtime_ms =
            old_runtime_ms +
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock().now() - start)
                .count();
    }

    return solutions;
}

solution_t solve_local_search_gen_greedy_swap(const tsp_t &tsp, unsigned int n,
                                              unsigned int start) {

    return solve_local_search(tsp, n, start, solution_t::SWAP, GREEDY);
}

solution_t solve_local_search_gen_greedy_reverse(const tsp_t &tsp,
                                                 unsigned int n,
                                                 unsigned int start) {
    return solve_local_search(tsp, n, start, solution_t::REVERSE, GREEDY);
}

solution_t solve_local_search_gen_steepest_swap(const tsp_t &tsp,
                                                unsigned int n,
                                                unsigned int start) {
    return solve_local_search(tsp, n, start, solution_t::SWAP, STEEPEST);
}

solution_t solve_local_search_gen_steepest_reverse(const tsp_t &tsp,
                                                   unsigned int n,
                                                   unsigned int start) {
    return solve_local_search(tsp, n, start, solution_t::REVERSE, STEEPEST);
}

std::vector<solution_t> solve_local_search_random_greedy_swap(const tsp_t &tsp,
                                                              unsigned int n) {
    return solve_local_search(tsp, n, solution_t::SWAP, GREEDY);
}

std::vector<solution_t>
solve_local_search_random_greedy_reverse(const tsp_t &tsp, unsigned int n) {
    return solve_local_search(tsp, n, solution_t::REVERSE, GREEDY);
}

std::vector<solution_t>
solve_local_search_random_steepest_swap(const tsp_t &tsp, unsigned int n) {
    return solve_local_search(tsp, n, solution_t::SWAP, STEEPEST);
}

std::vector<solution_t>
solve_local_search_random_steepest_reverse(const tsp_t &tsp, unsigned int n) {
    return solve_local_search(tsp, n, solution_t::REVERSE, STEEPEST);
}
