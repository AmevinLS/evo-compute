#pragma once

#include <vector>

#include "../common/random.cpp"
#include "../common/types.cpp"
#include "../task2/solve_greedy_regret.cpp"

class RecombOper {
  public:
    RecombOper(const tsp_t &tsp) : tsp_(tsp) {}

    virtual solution_t Recombine(const solution_t &sol1,
                                 const solution_t &sol2) const = 0;
    virtual ~RecombOper() {}

    const tsp_t &tsp_;
};

class RandomFillOper : public RecombOper {
  public:
    using RecombOper::RecombOper;

    solution_t Recombine(const solution_t &sol1, const solution_t &sol2) const {
        // TODO
    };
};

class HeuristicRepairOper : public RecombOper {
  public:
    using RecombOper::RecombOper;

    solution_t Recombine(const solution_t &sol1, const solution_t &sol2) const {
        // TODO: really not sure if this is correct
        bool rand_bool = random_num(0, 2) == 0;
        const solution_t &primary_sol = rand_bool ? sol1 : sol2,
                         secondary_sol = rand_bool ? sol2 : sol1;
        std::vector<unsigned> res_path;
        for (unsigned node : primary_sol.path) {
            if (secondary_sol.remaining_nodes.count(node) > 0)
                continue;
            res_path.push_back(node);
        }
        solution_t res_sol(tsp_, res_path);
        res_sol = solve_regret(res_sol, 100, 0.5);
        return res_sol;
    }
};