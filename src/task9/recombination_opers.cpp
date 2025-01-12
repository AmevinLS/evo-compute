#pragma once

#include <vector>

#include "../common/random.cpp"
#include "../common/types.cpp"
#include "../task2/solve_greedy_regret.cpp"

auto rd = std::random_device();
auto rng = std::default_random_engine(rd());

std::vector<unsigned int> combine(const solution_t &left,
                                  const solution_t &right, bool fill) {
    auto edge_set = left.to_edges();
    std::vector<unsigned int> new_path;
    std::unordered_set<unsigned int> rn;
    bool prev = false;
    bool curr = false;

    if (fill) {
        for (unsigned int i = 0; i < left.tsp->n; i++) {
            rn.insert(i);
        }
    }

    for (int i = 0; i < right.path.size(); i++) {
        curr = edge_set.count(
                   edge_t{right.path[i], right.path[right.next(i)]}) > 0;

        if (prev || curr) {
            new_path.push_back(right.path[i]);
            if (fill) {
                rn.erase(right.path[i]);
            }
        } else if (fill) {
            new_path.push_back(UINT_MAX);
        }

        prev = curr;
    }

    if (fill) {
        std::vector<unsigned int> rnv =
            std::vector<unsigned int>(rn.begin(), rn.end());

        std::shuffle(rnv.begin(), rnv.end(), rng);
        int idx = 0;

        for (int i = 0; i < new_path.size(); i++) {
            if (new_path[i] != UINT_MAX) {
                continue;
            }
            new_path[i] = rnv[idx++];
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
