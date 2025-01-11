#include "../common/parse.cpp"
#include "../common/print.cpp"
#include "../common/types.cpp"
#include "../task6/solve_local_multiple.cpp"

#include "recombination_opers.cpp"
#include "solve_hybrid_evolutionary.cpp"
#include <memory>

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
    std::cout << std::endl;
    std::cout << "MSLS:" << std::endl << solutions_msls << std::endl;

    std::cout << "\n";
    std::vector<std::unique_ptr<RecombOper>> recomb_options;
    recomb_options.push_back(std::make_unique<RandomFillOper>(tsp));
    recomb_options.push_back(std::make_unique<HeuristicRepairOper>(tsp));
    for (bool ls_after_repair : {false, true}) {
        for (const auto &recomb_oper : recomb_options) {
            std::vector<solution_t> solutions_lns;
            for (unsigned i = 0; i < NUM_RUNS; i++) {
                timer.start();
                solution_t evo_sol = SolveHybridEvolutionary(
                    tsp, PATH_SIZE, 20, *recomb_oper, true, avg_msls_duration);
                evo_sol.runtime_ms = timer.measure();
                solutions_lns.push_back(evo_sol);
                std::cout << "LNS with ls_after_repair=" << ls_after_repair
                          << " " << i << ": " << evo_sol.cost << "\n";
            }
            std::cout << std::endl;
            std::cout << "LNS with ls_after_repair=" << ls_after_repair << ":"
                      << std::endl
                      << solutions_lns << std::endl;
        }
    }
}