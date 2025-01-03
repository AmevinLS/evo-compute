#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

#include "../common/parse.cpp"
#include "../common/print.cpp"
#include "../common/types.cpp"
#include "solve_local_iterated.cpp"

int main() {
    std::ifstream fin("../../data/TSPA.csv");
    tsp_t tsp = parse(fin);
    timer_t timer;

    const unsigned NUM_RUNS = 10;
    const unsigned PATH_SIZE = 100;

    std::vector<solution_t> solutions_msls;
    for (unsigned i = 0; i < NUM_RUNS; i++) {
        timer.start();
        solution_t sol = local_search_multiple_start(tsp, PATH_SIZE);
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
        solution_t iterated_sol =
            local_search_iterated(tsp, PATH_SIZE, avg_msls_duration);
        iterated_sol.runtime_ms = timer.measure();
        solutions_iter.push_back(iterated_sol);
        std::cout << "Iterated " << i << ": " << iterated_sol.cost << "\n";
    }

    std::cout << std::endl;
    std::cout << "MSLS:" << std::endl << solutions_msls << std::endl;
    std::cout << solutions_msls;
    std::cout << "Iterated:" << std::endl << solutions_iter << std::endl;
}
