#include <chrono>

#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task3/solve_local_search.cpp"

struct multiple_start_result_t {
    solution_t solution;
    int duration_ms;
};

multiple_start_result_t solve_local_multiple_start(const tsp_t &tsp,
                                                   unsigned total_iters,
                                                   unsigned path_size = 100) {
    const auto start_time = std::chrono::high_resolution_clock::now();
    int best_cost = INT_MAX;
    std::optional<solution_t> best_sol;
    for (unsigned i = 0; i < total_iters; i++) {
        solution_t sol = solve_local_search(gen_random_solution(tsp, path_size),
                                            solution_t::REVERSE, STEEPEST);
        if (!best_sol || sol.cost < best_cost) {
            best_cost = sol.cost;
            best_sol = sol;
        }
    }
    const auto end_time = std::chrono::high_resolution_clock::now();
    int duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          end_time - start_time)
                          .count();
    return {best_sol.value(), duration_ms};
}