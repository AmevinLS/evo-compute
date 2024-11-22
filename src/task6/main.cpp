#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <vector>

#include "../common/parse.cpp"
#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task5/solve_local_deltas.cpp"
#include "solve_local_iterated.cpp"
#include "solve_local_multiple.cpp"

struct timer_t {
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

    void start() { start_time = std::chrono::high_resolution_clock::now(); }

    int measure() {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                     start_time)
            .count();
    }
};

std::string get_solutions_stats(const std::vector<solution_t> &sols) {
    int min_cost = INT_MAX, max_cost = 0;
    int min_time = INT_MAX, max_time = 0;
    double avg_cost, avg_time = 0;
    std::for_each(sols.cbegin(), sols.cend(), [&](const solution_t &sol) {
        avg_cost += sol.cost;
        avg_time += sol.runtime_ms;
        min_cost = std::min(sol.cost, min_cost);
        max_cost = std::max(sol.cost, max_cost);
        min_time = std::min(sol.runtime_ms, min_time);
        max_time = std::max(sol.runtime_ms, max_time);
    });
    avg_cost /= sols.size();
    avg_time /= sols.size();

    std::stringstream ss;
    ss << "Cost: " << min_cost << " / " << avg_cost << " / " << max_cost
       << "\n";
    ss << "Time: " << min_time << " / " << avg_time << " / " << max_time
       << "\n";
    return ss.str();
}

int main() {
    std::ifstream fin("../../data/TSPA.csv");
    tsp_t tsp = parse(fin);
    timer_t timer;

    const unsigned NUM_RUNS = 10;
    const unsigned MSLS_ITERATIONS = 200;
    const unsigned PATH_SIZE = 100;

    std::vector<solution_t> solutions_msls;
    for (unsigned i = 0; i < NUM_RUNS; i++) {
        timer.start();
        solution_t sol = solve_local_multiple_start(tsp, MSLS_ITERATIONS);
        sol.runtime_ms = timer.measure();
        solutions_msls.push_back(sol);
        std::cout << "MSLS " << i << ": " << sol.cost << "\n";
    }
    int avg_msls_duration =
        std::accumulate(solutions_msls.cbegin(), solutions_msls.cend(), 0,
                        [](int val, const solution_t &sol) {
                            return val + sol.runtime_ms;
                        }) /
        NUM_RUNS;

    std::cout << "\n";
    std::vector<solution_t> solutions_iter;
    for (unsigned i = 0; i < NUM_RUNS; i++) {
        timer.start();
        solution_t random_sol = gen_random_solution(tsp, PATH_SIZE);
        solution_t iterated_sol =
            solve_local_iterated_search(random_sol, avg_msls_duration);
        iterated_sol.runtime_ms = timer.measure();
        solutions_iter.push_back(iterated_sol);
        std::cout << "Iterated " << i << ": " << iterated_sol.cost << "\n";
    }

    std::cout << "\n";
    std::cout << "MSLS:\n";
    std::cout << get_solutions_stats(solutions_msls) << "\n";
    std::cout << "Iterated:\n";
    std::cout << get_solutions_stats(solutions_iter);

    // std::vector<solution_t> init_sols = solve_random(tsp, 100);
    // solution_t sol = init_sols[0];

    // std::cout << "Starting experiment" << "\n";
    // for (unsigned i = 0; i < 1000; i++) {
    //     if (i % 100 == 0)
    //         std::cout << "Iteration " << i << "\n";
    //     int sol_idx = 0;
    //     for (const solution_t &sol : init_sols) {
    //         // solution_t search_res =
    //         //     solve_local_search(sol, solution_t::REVERSE, STEEPEST);
    //         solution_t delta_res = local_deltas_steepest(tsp, sol);
    //         // if (search_res.cost != delta_res.cost)
    //         //     std::cout << sol_idx << ": " << search_res.cost
    //         //               << " != " << delta_res.cost << "\n";
    //         sol_idx++;
    //     }
    // }
    // std::cout << "Finished experiment" << "\n";

    // solution_t search_res =
    //     solve_local_search(init_sols[199], solution_t::REVERSE, STEEPEST);
    // std::cout << "Search final cost: " << search_res.cost << "\n";
    // solution_t delta_res = local_deltas_steepest(tsp, init_sols[199]);
    // std::cout << "Deltas final cost: " << delta_res.cost << "\n";
}