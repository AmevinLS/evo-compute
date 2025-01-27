#pragma once

#include "../common/types.cpp"
#include "task1.cpp"
#include "task10.cpp"
#include "task2.cpp"
#include "task3.cpp"
#include "task4.cpp"
#include "task5.cpp"
#include "task6.cpp"
#include "task7.cpp"
#include "task9.cpp"

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#define NUM_RUNS 20

typedef std::function<solution_t(const tsp_t &, unsigned int)> heuristic_fn_t;
typedef std::function<solution_t(solution_t)> random_fn_t;
typedef std::function<solution_t(const tsp_t &, unsigned int)> iterated_fn_t;

#pragma region Solvers

std::vector<solution_t> solve_heuristic(const tsp_t &tsp, heuristic_fn_t fn) {
    std::vector<solution_t> solutions;
    solutions.reserve(tsp.n);

    timer_t timer;

    for (unsigned int i = 0; i < tsp.n; i++) {
        timer.start();
        solution_t solution = fn(tsp, i);
        solution.runtime_ms = timer.measure();
        solutions.push_back(solution);
    }

    return solutions;
}

std::vector<solution_t> solve_random(const tsp_t &tsp, random_fn_t fn) {
    std::vector<solution_t> solutions = random(tsp);

    timer_t timer;
    for (unsigned int i = 0; i < solutions.size(); i++) {
        std::cout << "Solving random " << i << std::endl;
        timer.start();
        solutions[i] = fn(solutions[i]);
        solutions[i].runtime_ms = timer.measure();
    }

    return solutions;
}

std::vector<solution_t>
solve_iterated(const tsp_t &tsp, iterated_fn_t fn,
               std::optional<unsigned int> time_limit_ms = std::nullopt) {
    unsigned int tl = time_limit_ms.value_or(calc_time_limit_ms(tsp));
    std::vector<solution_t> solutions;
    solutions.reserve(NUM_RUNS);
    timer_t timer;

    for (int i = 0; i < NUM_RUNS; i++) {
        timer.start();
        solution_t solution = fn(tsp, tl);
        solution.runtime_ms = timer.measure();
        solutions.push_back(solution);
    }

    return solutions;
}

#pragma endregion Solvers

#pragma region Algorithms

struct random_algo : algo_t {
    std::string short_name() const override { return "random"; }
    std::string full_name() const override { return "Random"; }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return random(tsp);
    }
};

struct nn_end_algo : algo_t {
    std::string short_name() const override { return "nn_end"; }
    std::string full_name() const override { return "Nearest Neighbor (End)"; }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_heuristic(tsp, nn_end);
    }
};

struct nn_any_algo : algo_t {
    std::string short_name() const override { return "nn_any"; }
    std::string full_name() const override { return "Nearest Neighbor (Any)"; }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_heuristic(tsp, nn_any);
    }
};

struct greedy_cycle_algo : algo_t {
    std::string short_name() const override { return "greedy_cycle"; }
    std::string full_name() const override { return "Greedy Cycle"; }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_heuristic(tsp, greedy_cycle);
    }
};

struct greedy_cycle_regret_unweighted_algo : algo_t {
    std::string short_name() const override {
        return "greedy_cycle_regret_unweighted";
    }
    std::string full_name() const override {
        return "Greedy Cycle /w Regret (Unweighted)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_heuristic(tsp, fn);
    }

    static solution_t fn(const tsp_t &tsp, unsigned int start) {
        solution_t solution(tsp, find_cycle(tsp, start));
        return greedy_cycle_regret(solution, 1.0);
    }
};

struct greedy_cycle_regret_weighted_algo : algo_t {
    std::string short_name() const override {
        return "greedy_cycle_regret_weighted";
    }
    std::string full_name() const override {
        return "Greedy Cycle /w Regret (Weighted)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_heuristic(tsp, fn);
    }

    static solution_t fn(const tsp_t &tsp, unsigned int start) {
        solution_t solution(tsp, find_cycle(tsp, start));
        return greedy_cycle_regret(solution, REGRET_WEIGHT);
    }
};

struct local_search_gen_greedy_swap_algo : algo_t {
    std::string short_name() const override {
        return "local_search_gen_greedy_swap";
    }
    std::string full_name() const override {
        return "Local Search (Heuristic, Greedy, Node Swap)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_heuristic(tsp, fn);
    }

    static solution_t fn(const tsp_t &tsp, unsigned int start) {
        solution_t solution = greedy_cycle_regret_weighted_algo::fn(tsp, start);
        return local_search(solution, SWAP, GREEDY);
    }
};

struct local_search_gen_greedy_reverse_algo : algo_t {
    std::string short_name() const override {
        return "local_search_gen_greedy_reverse";
    }
    std::string full_name() const override {
        return "Local Search (Heuristic, Greedy, Node Reverse)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_heuristic(tsp, fn);
    }

    static solution_t fn(const tsp_t &tsp, unsigned int start) {
        solution_t solution = greedy_cycle_regret_weighted_algo::fn(tsp, start);
        return local_search(solution, REVERSE, GREEDY);
    }
};

struct local_search_gen_steepest_swap_algo : algo_t {
    std::string short_name() const override {
        return "local_search_gen_steepest_swap";
    }
    std::string full_name() const override {
        return "Local Search (Heuristic, Steepest, Node Swap)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_heuristic(tsp, fn);
    }

    static solution_t fn(const tsp_t &tsp, unsigned int start) {
        solution_t solution = greedy_cycle_regret_weighted_algo::fn(tsp, start);
        return local_search(solution, SWAP, STEEPEST);
    }
};

struct local_search_gen_steepest_reverse_algo : algo_t {
    std::string short_name() const override {
        return "local_search_gen_steepest_reverse";
    }
    std::string full_name() const override {
        return "Local Search (Heuristic, Steepest, Node Reverse)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_heuristic(tsp, fn);
    }

    static solution_t fn(const tsp_t &tsp, unsigned int start) {
        solution_t solution = greedy_cycle_regret_weighted_algo::fn(tsp, start);
        return local_search(solution, REVERSE, STEEPEST);
    }
};

struct local_search_random_greedy_swap_algo : algo_t {
    std::string short_name() const override {
        return "local_search_random_greedy_swap";
    }
    std::string full_name() const override {
        return "Local Search (Random, Greedy, Node Swap)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_random(tsp, fn);
    }

    static solution_t fn(solution_t solution) {
        return local_search(solution, SWAP, GREEDY);
    }
};

struct local_search_random_greedy_reverse_algo : algo_t {
    std::string short_name() const override {
        return "local_search_random_greedy_reverse";
    }
    std::string full_name() const override {
        return "Local Search (Random, Greedy, Node Reverse)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_random(tsp, fn);
    }

    static solution_t fn(solution_t solution) {
        return local_search(solution, REVERSE, GREEDY);
    }
};

struct local_search_random_steepest_swap_algo : algo_t {
    std::string short_name() const override {
        return "local_search_random_steepest_swap";
    }
    std::string full_name() const override {
        return "Local Search (Random, Steepest, Node Swap)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_random(tsp, fn);
    }

    static solution_t fn(solution_t solution) {
        return local_search(solution, SWAP, STEEPEST);
    }
};

struct local_search_random_steepest_reverse_algo : algo_t {
    std::string short_name() const override {
        return "local_search_random_steepest_reverse";
    }
    std::string full_name() const override {
        return "Local Search (Random, Steepest, Node Reverse)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_random(tsp, fn);
    }

    static solution_t fn(solution_t solution) {
        return local_search(solution, REVERSE, STEEPEST);
    }
};

struct local_candidates_algo : algo_t {
    std::string short_name() const override { return "local_candidates"; }
    std::string full_name() const override { return "Local Candidates"; }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_random(tsp, local_candidates);
    }
};

struct local_deltas_algo : algo_t {
    std::string short_name() const override { return "local_deltas"; }
    std::string full_name() const override { return "Local Deltas"; }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_random(tsp, local_deltas);
    }
};

struct local_search_multiple_start_algo : algo_t {
    std::string short_name() const override {
        return "local_search_multiple_start";
    }

    std::string full_name() const override {
        return "Local Search (Multiple Start)";
    }

    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_iterated(tsp, fn);
    }

    static solution_t fn(const tsp_t &tsp, unsigned int time_limit_ms) {
        return local_search_multiple_start(tsp);
    }
};

struct local_search_iterated_algo : algo_t {
    std::string short_name() const override { return "local_search_iterated"; }
    std::string full_name() const override { return "Local Search (Iterated)"; }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_iterated(tsp, local_search_iterated, time_limit_ms);
    }
};

struct large_neighborhood_search_no_ls_algo : algo_t {
    std::string short_name() const override {
        return "large_neighborhood_search_no_ls";
    }
    std::string full_name() const override {
        return "Large Neighborhood Search (No LS)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_iterated(tsp, fn, time_limit_ms);
    }

    static solution_t fn(const tsp_t &tsp, unsigned int time_limit_ms) {
        return large_neighborhood_search(tsp, time_limit_ms, false);
    }
};

struct large_neighborhood_search_ls_algo : algo_t {
    std::string short_name() const override {
        return "large_neighborhood_search_ls";
    }
    std::string full_name() const override {
        return "Large Neighborhood Search (LS)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_iterated(tsp, fn, time_limit_ms);
    }
    static solution_t fn(const tsp_t &tsp, unsigned int time_limit_ms) {
        return large_neighborhood_search(tsp, time_limit_ms, true);
    }
};

struct hybrid_evo_fill_no_ls_algo : algo_t {
    std::string short_name() const override { return "hybrid_evo_fill_no_ls"; }
    std::string full_name() const override {
        return "Hybrid Evolutionary (Fill, No LS)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_iterated(tsp, fn, time_limit_ms);
    }
    static solution_t fn(const tsp_t &tsp, unsigned int time_limit_ms) {
        return hybrid_evolutionary(tsp, random_fill_op, false, time_limit_ms);
    }
};

struct hybrid_evo_fill_ls_algo : algo_t {
    std::string short_name() const override { return "hybrid_evo_fill_ls"; }
    std::string full_name() const override {
        return "Hybrid Evolutionary (Fill, LS)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_iterated(tsp, fn, time_limit_ms);
    }
    static solution_t fn(const tsp_t &tsp, unsigned int time_limit_ms) {
        return hybrid_evolutionary(tsp, random_fill_op, true, time_limit_ms);
    }
};

struct hybrid_evo_repair_no_ls_algo : algo_t {
    std::string short_name() const override {
        return "hybrid_evo_repair_no_ls";
    }
    std::string full_name() const override {
        return "Hybrid Evolutionary (Repair, No LS)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_iterated(tsp, fn, time_limit_ms);
    }
    static solution_t fn(const tsp_t &tsp, unsigned int time_limit_ms) {
        return hybrid_evolutionary(tsp, heuristic_repair_op, false,
                                   time_limit_ms);
    }
};

struct hybrid_evo_repair_ls_algo : algo_t {
    std::string short_name() const override { return "hybrid_evo_repair_ls"; }
    std::string full_name() const override {
        return "Hybrid Evolutionary (Repair, LS)";
    }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_iterated(tsp, fn, time_limit_ms);
    }
    static solution_t fn(const tsp_t &tsp, unsigned int time_limit_ms) {
        return hybrid_evolutionary(tsp, heuristic_repair_op, true,
                                   time_limit_ms);
    }
};

struct custom_algo : algo_t {
    std::string short_name() const override { return "custom"; }
    std::string full_name() const override { return "Custom"; }
    std::vector<solution_t> run(const tsp_t &tsp,
                                std::optional<unsigned int> time_limit_ms =
                                    std::nullopt) const override {
        return solve_iterated(tsp, custom_lns, time_limit_ms);
    }
};

#pragma endregion Algorithms

#pragma region Collections

template <typename T, typename... Args> auto make(Args &&...args) {
    std::vector<T> vec;
    vec.reserve(sizeof...(Args));
    (vec.emplace_back(std::forward<Args>(args)), ...);
    return vec;
}

std::vector<std::unique_ptr<algo_t>> DEFAULT_ALGOS =
    make<std::unique_ptr<algo_t>>(
        std::make_unique<random_algo>(), std::make_unique<nn_end_algo>(),
        std::make_unique<nn_any_algo>(), std::make_unique<greedy_cycle_algo>(),
        std::make_unique<greedy_cycle_regret_unweighted_algo>(),
        std::make_unique<greedy_cycle_regret_weighted_algo>(),
        std::make_unique<local_search_gen_greedy_swap_algo>(),
        std::make_unique<local_search_gen_greedy_reverse_algo>(),
        std::make_unique<local_search_gen_steepest_swap_algo>(),
        std::make_unique<local_search_gen_steepest_reverse_algo>(),
        std::make_unique<local_search_random_greedy_swap_algo>(),
        std::make_unique<local_search_random_greedy_reverse_algo>(),
        std::make_unique<local_search_random_steepest_swap_algo>(),
        std::make_unique<local_search_random_steepest_reverse_algo>(),
        std::make_unique<local_candidates_algo>(),
        std::make_unique<local_deltas_algo>(),
        std::make_unique<local_search_multiple_start_algo>(),
        std::make_unique<local_search_iterated_algo>(),
        std::make_unique<large_neighborhood_search_no_ls_algo>(),
        std::make_unique<large_neighborhood_search_ls_algo>(),
        std::make_unique<hybrid_evo_fill_no_ls_algo>(),
        std::make_unique<hybrid_evo_fill_ls_algo>(),
        std::make_unique<hybrid_evo_repair_no_ls_algo>(),
        std::make_unique<hybrid_evo_repair_ls_algo>(),
        std::make_unique<custom_algo>());

#pragma endregion Collections
