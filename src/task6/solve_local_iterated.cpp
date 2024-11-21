#include <chrono>
#include <random>

#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task3/solve_local_search.cpp"

int random_num(int start, int end) {
    std::random_device rd;  // obtain a random number from hardware
    std::mt19937 gen(rd()); // seed the generator
    std::uniform_int_distribution<> distr(start, end); // define the range
    return distr(gen);
}

// Perturb the solution in-place
void perturb_solution(solution_t &sol) {
    for (int i = 0; i < 3; i++) {
        auto iter = sol.remaining_nodes.begin();
        std::advance(iter, random_num(0, sol.remaining_nodes.size()));
        sol.replace(*iter, random_num(0, sol.path.size()));
    }
}

solution_t solve_local_iterated_search(solution_t solution,
                                       unsigned time_limit_ms) {
    int best_cost = INT_MAX;
    solution_t curr_sol = solution;

    const std::chrono::time_point start_time =
        std::chrono::high_resolution_clock::now();
    while (true) {
        curr_sol = solve_local_search(curr_sol, solution_t::REVERSE, STEEPEST);

        const std::chrono::time_point curr_time =
            std::chrono::high_resolution_clock::now();
        int curr_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          curr_time - start_time)
                          .count();
        if (curr_ms > time_limit_ms)
            break;

        if (curr_sol.cost < best_cost) {
            best_cost = curr_sol.cost;
            curr_sol = curr_sol;
        }
        perturb_solution(curr_sol);
    }
    return curr_sol;
}