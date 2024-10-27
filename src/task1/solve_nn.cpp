#include "../common/search.cpp"
#include "../common/types.cpp"

solution_t solve_nn_end(const tsp_t &tsp, unsigned int n, unsigned int start) {
    solution_t solution(tsp, start);

    while (solution.path.size() < n) {
        auto [min, min_delta] = find_nn(solution, [=](unsigned int node) {
            return solution.append_delta(node);
        });

        solution.append(min);
    }

    solution.commit();
    return solution;
}

solution_t solve_nn_any(const tsp_t &tsp, unsigned int n, unsigned int start) {
    solution_t solution(tsp, start);

    while (solution.path.size() < n) {
        unsigned int min_pos = -1;

        auto [min, min_delta] = find_nn(solution, [=](unsigned int node) {
            return solution.prepend_delta(node);
        });

        for (int i = 0; i < solution.path.size() - 1; i++) {
            auto [min_replace, min_replace_delta] = find_replace(solution, i);

            if (min_replace_delta < min_delta) {
                min = min_replace;
                min_delta = min_replace_delta;
                min_pos = i;
            }
        }

        auto [last, last_delta] = find_nn(solution, [=](unsigned int node) {
            return solution.append_delta(node);
        });

        if (last_delta < min_delta) {
            solution.append(last);
        } else if (min_pos == -1) {
            solution.prepend(min);
        } else {
            solution.insert(min, min_pos);
        }
    }

    solution.commit();
    return solution;
}
