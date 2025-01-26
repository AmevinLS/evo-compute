#pragma once

#include "../common/types.cpp"
#include "../task6/solve_local_multiple.cpp"
#include "../task9/recombination_opers.cpp"
#include "../task9/solve_hybrid_evolutionary.cpp"
#include <string>

class Algorithm {
  public:
    virtual solution_t Run(const tsp_t &tsp, unsigned path_size,
                           unsigned time_limit_ms) const = 0;
    virtual std::string GetName() = 0;

    virtual ~Algorithm() = default;
};

class MultipleStart : public Algorithm {
  public:
    solution_t Run(const tsp_t &tsp, unsigned path_size,
                   unsigned time_limit_ms) const {
        return local_search_multiple_start(tsp, path_size);
    }

    std::string GetName() { return "MultipleStart"; }
};

class HybridEvoRepair : public Algorithm {
  public:
    bool ls_;

    HybridEvoRepair(bool ls) : ls_(ls) {}

    solution_t Run(const tsp_t &tsp, unsigned path_size,
                   unsigned time_limit_ms) const {
        return solve_hybrid_evolutionary(
            tsp, path_size, 20, heuristic_repair_op, ls_, time_limit_ms);
    }

    std::string GetName() {
        std::string ls_as_str = (ls_ ? "true" : "false");
        return "HybridEvoRepair(ls=" + ls_as_str + ")";
    }
};
