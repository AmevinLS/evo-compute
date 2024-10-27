#include <iostream>
#include <optional>
#include <random>
#include <vector>

#include "../common/print.cpp"
#include "../common/search.cpp"
#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task2/solve_greedy_regret.cpp"

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

solution_t solve_local_search(solution_t start, intra_path_t intra_op_type,
                              search_t search_type) {
    solution_t cur = start;
    std::mt19937 g = std::mt19937(std::random_device()());

    std::cout << cur;

    while (true) {
        std::vector<operation_t> neighbourhood =
            find_neighbourhood(cur, intra_op_type);
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
        best_op.value().func(cur);
        std::cout << best_op.value().delta << "\n";
        // std::cout << "Cost after step: " << cur.cost << "\n";
        std::cout << cur;
    }

    return cur;
}

solution_t solve_local_search(const tsp_t &tsp, unsigned int n,
                              unsigned int start, intra_path_t intra_op_type,
                              search_t search_type) {
    solution_t solution = solve_regret_weighted(tsp, n, start);
    return solve_local_search(solution, intra_op_type, search_type);
}

std::vector<solution_t> solve_local_search(const tsp_t &tsp, unsigned int n,
                                           intra_path_t intra_op_type,
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

    return solve_local_search(tsp, n, start, SWAP, GREEDY);
}

solution_t solve_local_search_gen_greedy_reverse(const tsp_t &tsp,
                                                 unsigned int n,
                                                 unsigned int start) {
    return solve_local_search(tsp, n, start, REVERSE, GREEDY);
}

solution_t solve_local_search_gen_steepest_swap(const tsp_t &tsp,
                                                unsigned int n,
                                                unsigned int start) {
    return solve_local_search(tsp, n, start, SWAP, STEEPEST);
}

solution_t solve_local_search_gen_steepest_reverse(const tsp_t &tsp,
                                                   unsigned int n,
                                                   unsigned int start) {
    return solve_local_search(tsp, n, start, REVERSE, STEEPEST);
}

std::vector<solution_t> solve_local_search_random_greedy_swap(const tsp_t &tsp,
                                                              unsigned int n) {
    return solve_local_search(tsp, n, SWAP, GREEDY);
}

std::vector<solution_t>
solve_local_search_random_greedy_reverse(const tsp_t &tsp, unsigned int n) {
    return solve_local_search(tsp, n, REVERSE, GREEDY);
}

std::vector<solution_t>
solve_local_search_random_steepest_swap(const tsp_t &tsp, unsigned int n) {
    return solve_local_search(tsp, n, SWAP, STEEPEST);
}

std::vector<solution_t>
solve_local_search_random_steepest_reverse(const tsp_t &tsp, unsigned int n) {
    return solve_local_search(tsp, n, REVERSE, STEEPEST);
}
