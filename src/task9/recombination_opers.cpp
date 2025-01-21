#pragma once

#include "../common/random.cpp"
#include "../common/types.cpp"
#include "../task2/solve_greedy_regret.cpp"

#include <vector>

auto rd = std::random_device();
auto rng = std::default_random_engine(rd());

std::vector<unsigned int> combine(const solution_t &left,
                                  const solution_t &right, bool fill) {
    auto edge_set = left.to_edges();
    std::vector<unsigned int> new_path;
    std::vector<bool> rn = std::vector<bool>(left.tsp->n, true);
    bool prev = false;
    bool curr = false;

    new_path.reserve(left.path.size());

    for (int i = 0; i < right.path.size(); i++) {
        curr = edge_set.count(
                   edge_t{right.path[i], right.path[right.next(i)]}) > 0;

        if (prev || curr) {
            new_path.push_back(right.path[i]);
            rn[right.path[i]] = false;
        } else if (fill) {
            new_path.push_back(UINT_MAX);
        }

        prev = curr;
    }

    if (fill) {
        int idx = random_num(0, rn.size());

        for (int i = 0; i < new_path.size(); i++) {
            if (new_path[i] != UINT_MAX) {
                continue;
            }

            while (!rn[idx]) {
                idx = idx == rn.size() - 1 ? 0 : idx + 1;
            }

            new_path[i] = idx;
            rn[idx] = false;
        }
    }

    return new_path;
}

solution_t random_fill_op(const solution_t &sol1, const solution_t &sol2) {
    return solution_t(*(sol1.tsp), combine(sol1, sol2, true));
};

solution_t heuristic_repair_op(const solution_t &sol1, const solution_t &sol2) {
    std::vector<unsigned> new_path = combine(sol1, sol2, false);
    if (new_path.size() == 0) {
        new_path = find_cycle(*(sol1.tsp), random_num(0, 200));
    } else if (new_path.size() < 3) {
        unsigned start_node = new_path[random_num(0, new_path.size())];
        new_path = find_cycle(*(sol1.tsp), start_node);
    }

    solution_t res_sol(*(sol1.tsp), new_path);
    res_sol = solve_regret(res_sol, 100, 0.5);
    return res_sol;
}
