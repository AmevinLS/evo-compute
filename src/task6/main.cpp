#include <fstream>
#include <iostream>

#include "../common/parse.cpp"
#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task3/solve_local_search.cpp"
#include "../task5/solve_local_deltas.cpp"
#include "solve_local_iterated.cpp"
#include "solve_local_multiple.cpp"

int main() {
    std::ifstream fin("../data/TSPA.csv");
    tsp_t tsp = parse(fin);

    const unsigned NUM_RUNS = 20;
    const unsigned MSLS_ITERATIONS = 200;
    const unsigned PATH_SIZE = 100;

    int msls_total_duration = 0;
    for (unsigned i = 0; i < NUM_RUNS; i++) {
        const auto start_time = std::chrono::high_resolution_clock::now();
        solve_local_multiple_start(tsp, MSLS_ITERATIONS);
        const auto end_time = std::chrono::high_resolution_clock::now();
        int duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                              end_time - start_time)
                              .count();
        msls_total_duration += duration_ms;
    }
    int avg_msls_duration = msls_total_duration / NUM_RUNS;

    for (unsigned i = 0; i < NUM_RUNS; i++) {
        solution_t random_sol = gen_random_solution(tsp, PATH_SIZE);
        solution_t iterated_sol =
            solve_local_iterated_search(random_sol, avg_msls_duration);
    }

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