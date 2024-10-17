#include "common/types.cpp"
#include "task1/solve_greedy_cycle.cpp"
#include "task1/solve_nn.cpp"
#include "task1/solve_random.cpp"
#include "task2/solve_greedy_regret.cpp"

#include <map>
#include <vector>

enum heuristic_t { RANDOM, NN_END, NN_ANY, GREEDY_CYCLE, GREEDY_CYCLE_REGRET };

std::map<heuristic_t, std::string> heuristic_t_str = {
    {RANDOM, "random"},
    {NN_END, "nn_end"},
    {NN_ANY, "nn_any"},
    {GREEDY_CYCLE, "greedy_cycle"},
    {GREEDY_CYCLE_REGRET, "greedy_cycle_regret"}};

std::vector<solution_t> solve(const tsp_t &tsp, heuristic_t heuristic) {
    std::vector<solution_t> solutions;
    unsigned int n = ceil(tsp.n / 2.0);

    if (heuristic == RANDOM) {
        return solve_random(tsp, n);
    }

    for (unsigned int i = 0; i < tsp.n; i++) {
        switch (heuristic) {
        case RANDOM:
            // NOTE: should never reach this point, but it's here to avoid a
            // warning
            break;
        case NN_END:
            solutions.push_back(solve_nn_end(tsp, n, i));
            break;
        case NN_ANY:
            solutions.push_back(solve_nn_any(tsp, n, i));
            break;
        case GREEDY_CYCLE:
            solutions.push_back(run_greedy_cycle(tsp, n, i));
            break;
        case GREEDY_CYCLE_REGRET:
            CycleRegretSolver solver(tsp);
            solutions.push_back(solver.solve(n, i, 2));
            break;
        }
    }

    return solutions;
}
