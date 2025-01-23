#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task6/solve_local_multiple.cpp"
#include "../task9/recombination_opers.cpp"
#include "../task9/solve_hybrid_evolutionary.cpp"
#include <string>

class Algorithm {
  public:
    virtual solution_t Run(const tsp_t &tsp, unsigned path_size,
                           unsigned time_limit_ms) const = 0;
    virtual std::string GetName() = 0;
};

class MultipleStart : public Algorithm {
  public:
    solution_t Run(const tsp_t &tsp, unsigned path_size,
                   unsigned time_limit_ms) const {
        return local_search_multiple_start(tsp, path_size);
    }

    std::string GetName() { return "MultipleStart"; }
};

class HybridEvoRepairNoLS : public Algorithm {
  public:
    solution_t Run(const tsp_t &tsp, unsigned path_size,
                   unsigned time_limit_ms) const {
        return solve_hybrid_evolutionary(
            tsp, path_size, 20, heuristic_repair_op, false, time_limit_ms);
    }

    std::string GetName() { return "HybridEvoRepairNoLS"; }
};

class CustomAlgo : public Algorithm {
  public:
    solution_t Run(const tsp_t &tsp, unsigned path_size,
                   unsigned time_limit_ms) const {
        return gen_random_solution(tsp, path_size);
    }

    std::string GetName() { return "CustomAlgo"; }
};