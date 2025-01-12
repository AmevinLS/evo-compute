#include "../common/parse.cpp"
#include "../common/print.cpp"
#include "../common/types.cpp"
#include "../task6/solve_local_multiple.cpp"

#include "recombination_opers.cpp"
#include "solve_hybrid_evolutionary.cpp"
#include <utility>

int main() {
    std::ifstream fin("data/TSPA.csv");
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

    auto operations = {std::make_pair(random_fill_op, "RandomFill"),
                       std::make_pair(heuristic_repair_op, "HeuristicRepair")};

    for (bool ls_after_repair : {false, true}) {
        for (const auto &recomb_oper : operations) {
            std::vector<solution_t> solutions_lns;
            for (unsigned i = 0; i < NUM_RUNS; i++) {
                timer.start();
                solution_t evo_sol = solve_hybrid_evolutionary(
                    tsp, PATH_SIZE, 20, recomb_oper.first, ls_after_repair,
                    avg_msls_duration);
                evo_sol.runtime_ms = timer.measure();
                solutions_lns.push_back(evo_sol);
                std::cout << "HAE(" << recomb_oper.second
                          << ", ls_after_repair=" << ls_after_repair << ") "
                          << i << ": " << evo_sol.cost << "\n";
            }
            std::cout << std::endl;
            std::cout << "HAE(" << recomb_oper.second
                      << ", ls_after_repair=" << ls_after_repair << ") "
                      << ": " << std::endl
                      << solutions_lns << std::endl;
        }
    }
}
