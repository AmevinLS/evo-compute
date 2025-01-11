#pragma once

#include "../common/random.cpp"
#include "../common/types.cpp"
#include "../task1/solve_random.cpp"
#include "../task3/solve_local_search.cpp"
#include <iterator>
#include <set>
#include <utility>
#include <vector>

// This class is to avoid constructing pop_weights_ all the time or keeping all
// in the main func
class ParentSelector {
  public:
    ParentSelector(unsigned pop_size) : pop_weights_(pop_size, 1) {}

    std::pair<unsigned, unsigned> Select() {
        unsigned par1 = random_num(pop_weights_);
        pop_weights_[par1] = 0;
        unsigned par2 = random_num(pop_weights_);
        pop_weights_[par1] = 1;
        return {par1, par2};
    }

  private:
    std::vector<int> pop_weights_;
};

bool CheckSolInPopulation(const std::vector<solution_t> population,
                          const solution_t &sol) {
    for (const solution_t &other_sol : population) {
        if (other_sol.cost == sol.cost)
            return true;
    }
    return false;
}

class PopulationCostsTracker {
  public:
    PopulationCostsTracker(const std::vector<solution_t> &population) {}

    struct CostIdxPair {
        unsigned cost;
        unsigned idx;
    };

    CostIdxPair GetWorst() const { return *pairs_set_.rbegin(); }
    CostIdxPair GetBest() const { return *pairs_set_.begin(); }

    unsigned CostCount(unsigned cost) { return pairs_set_.count({cost, 0}); }

    void Insert(unsigned cost, unsigned idx) { pairs_set_.insert({cost, idx}); }

    void ReplaceWorst(unsigned new_cost, unsigned new_idx) {
        if (!pairs_set_.empty()) {
            pairs_set_.erase(std::prev(pairs_set_.cend()));
        }
        pairs_set_.insert({new_cost, new_idx});
    }

  private:
    struct CompareCostIdxPair {
        bool operator()(const CostIdxPair &a, const CostIdxPair &b) const {
            return a.cost < b.cost;
        }
    };

    std::multiset<CostIdxPair, CompareCostIdxPair> pairs_set_;
};

solution_t SolveHybridEvolutionary(
    const tsp_t &tsp, unsigned path_size, unsigned pop_size,
    solution_t (*recomb_oper)(const solution_t &, const solution_t &),
    bool ls_after_recomb, unsigned time_limit_ms) {
    std::vector<solution_t> population;
    population.reserve(pop_size);
    ParentSelector selector(pop_size);
    PopulationCostsTracker cost_tracker(population);

    for (unsigned i = 0; i < pop_size; i++) {
        solution_t sol = gen_random_solution(tsp, path_size);
        sol = solve_local_search(sol, solution_t::REVERSE, GREEDY);
        population.push_back(sol);
        cost_tracker.Insert(sol.cost, i);
    }

    timer_t timer;
    timer.start();
    unsigned search_iters = 0;
    while (timer.measure() < time_limit_ms) {
        // Draw at random two different solutions
        auto [par1, par2] = selector.Select();
        // Construct offspring by recombining parents
        solution_t child = recomb_oper(population[par1], population[par2]);
        if (ls_after_recomb)
            child = solve_local_search(child, solution_t::REVERSE, GREEDY);

        // Replace worst solution in population if better
        auto [worst_cost, worst_idx] = cost_tracker.GetWorst();
        if (child.cost < worst_cost &&
            cost_tracker.CostCount(child.cost) == 0) {
            population[worst_idx] = child;
            cost_tracker.ReplaceWorst(child.cost, worst_idx);
        }
        search_iters++;
    }

    solution_t res_sol = population[cost_tracker.GetBest().idx];
    res_sol.search_iters = search_iters;
    return res_sol;
}
