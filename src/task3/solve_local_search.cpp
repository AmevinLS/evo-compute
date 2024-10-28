#include "../common/search.cpp"
#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task2/solve_greedy_regret.cpp"

#include <optional>
#include <random>
#include <vector>

enum search_t { GREEDY, STEEPEST };

std::optional<operation_t>
greedy_search(const std::vector<operation_t> &operations) {
    for (operation_t operation : operations) {
        if (operation.delta < 0) {
            return operation;
        }
    }

    return std::nullopt;
}

std::optional<operation_t>
steepest_search(const std::vector<operation_t> &operations) {
    int best_diff = 0;
    std::optional<operation_t> best_op;
    for (operation_t operation : operations) {
        if (operation.delta < best_diff) {
            best_op = operation;
            best_diff = operation.delta;
        }
    }
    return best_op;
}

solution_t solve_local_search(solution_t solution,
                              solution_t::op_type_t op_type,
                              search_t search_type) {
    std::mt19937 g = std::mt19937(std::random_device()());

    while (true) {
        std::vector<operation_t> neighbourhood =
            find_neighbourhood(solution, op_type);
        std::shuffle(neighbourhood.begin(), neighbourhood.end(), g);

        std::optional<operation_t> best_op;
        if (search_type == GREEDY) {
            best_op = greedy_search(neighbourhood);
        } else if (search_type == STEEPEST) {
            best_op = steepest_search(neighbourhood);
        }

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
        solutions[i] =
            solve_local_search(solutions[i], intra_op_type, search_type);
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
