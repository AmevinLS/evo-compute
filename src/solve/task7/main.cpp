#include <fstream>
#include <iostream>
#include <numeric>
#include <vector>

#include "../common/parse.cpp"
#include "../common/print.cpp"
#include "../common/types.cpp"
#include "../task6/solve_local_multiple.cpp"
#include "solve_large_neighbors.cpp"

int main() {
    std::ifstream fin("../../data/TSPA.csv");
    tsp_t tsp = parse(fin);
    timer_t timer;

    const unsigned NUM_RUNS = 20;
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
    std::cout << std::endl;
    std::cout << "MSLS:" << std::endl << solutions_msls << std::endl;

    std::cout << "\n";
    for (bool ls_after_repair : {false, true}) {
        std::vector<solution_t> solutions_lns;
        for (unsigned i = 0; i < NUM_RUNS; i++) {
            timer.start();
            solution_t lns_sol =
                CustomLNS(tsp, PATH_SIZE, avg_msls_duration, true);
            lns_sol.runtime_ms = timer.measure();
            solutions_lns.push_back(lns_sol);
            std::cout << "LNS with ls_after_repair=" << ls_after_repair << " "
                      << i << ": " << lns_sol.cost << "\n";
        }
        std::cout << std::endl;
        std::cout << "LNS with ls_after_repair=" << ls_after_repair << ":"
                  << std::endl
                  << solutions_lns << std::endl;
    }
}
