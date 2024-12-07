#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task2/solve_greedy_regret.cpp"
#include "../task3/solve_local_search.cpp"
#include "../task6/solve_local_multiple.cpp"
#include <vector>

solution_t destroy_solution(solution_t sol) {
    unsigned num_nodes_to_remove =
        static_cast<unsigned>(sol.path.size() * 0.25);

    for (unsigned i = 0; i < num_nodes_to_remove; i++) {
        unsigned idx_to_remove = random_num(0, sol.path.size());
        sol.remove_node(idx_to_remove);
    }
    return sol;
}

solution_t large_neighborhood_search(const tsp_t &tsp, unsigned int path_size,
                                     unsigned int time_limit_ms,
                                     bool ls_after_repair) {
    solution_t solution = gen_random_solution(tsp, path_size);

    timer_t timer;
    timer.start();
    solution = solve_local_search(solution, solution_t::REVERSE, STEEPEST);
    solution_t best = solution;
    int i = 1;
    while (timer.measure() < time_limit_ms) {
        solution = destroy_solution(best); // Destroy solution
        solution =
            solve_regret(solution, path_size, REGRET_WEIGHT); // Repair solution
        if (ls_after_repair)
            solution =
                solve_local_search(solution, solution_t::REVERSE, STEEPEST);

        if (solution.cost < best.cost) {
            best = solution;
        }

        i++;
    }

    best.search_iters = i;
    return best;
}

std::vector<solution_t> solve_large_neighborhood_search(const tsp_t &tsp,
                                                        unsigned int path_size,
                                                        bool ls_after_repair) {
    std::vector<solution_t> mslp_solutions =
        solve_local_search_multiple_start(tsp, path_size);
    int time_limit_ms =
        std::accumulate(mslp_solutions.begin(), mslp_solutions.end(), 0,
                        [](int val, const solution_t &sol) {
                            return val + sol.runtime_ms;
                        }) /
        mslp_solutions.size();

    std::vector<solution_t> solutions;
    solutions.reserve(20);
    timer_t timer;
    for (unsigned int i = 0; i < 20; i++) {
        timer.start();
        solutions.push_back(large_neighborhood_search(
            tsp, path_size, time_limit_ms, ls_after_repair));
        solutions.back().runtime_ms = timer.measure();
        solutions.back().search_iters = tsp.n;
    }
    return solutions;
}