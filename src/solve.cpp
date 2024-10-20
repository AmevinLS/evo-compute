#include "common/types.cpp"
#include "task1/solve_greedy_cycle.cpp"
#include "task1/solve_nn.cpp"
#include "task1/solve_random.cpp"
#include "task2/solve_greedy_regret.cpp"

#include <map>
#include <vector>

enum heuristic_t {
  RANDOM,
  NN_END,
  NN_ANY,
  GREEDY_CYCLE,
  GREEDY_CYCLE_REGRET,
  GREEDY_CYCLE_REGRET_WEIGHTED
};

std::map<heuristic_t, std::string> heuristic_t_str = {
    {RANDOM, "random"},
    {NN_END, "nn_end"},
    {NN_ANY, "nn_any"},
    {GREEDY_CYCLE, "greedy_cycle"},
    {GREEDY_CYCLE_REGRET, "greedy_cycle_regret"},
    {GREEDY_CYCLE_REGRET_WEIGHTED, "greedy_cycle_regret_weighted"}};

std::map<heuristic_t, solution_t (*)(const tsp_t &, unsigned int, unsigned int)>
    heuristic_t_fn = {{NN_END, solve_nn_end},
                      {NN_ANY, solve_nn_any},
                      {GREEDY_CYCLE, solve_greedy_cycle},
                      {GREEDY_CYCLE_REGRET, solve_regret_unweighted},
                      {GREEDY_CYCLE_REGRET_WEIGHTED, solve_regret_weighted}};

std::vector<solution_t> solve(const tsp_t &tsp, heuristic_t heuristic) {
  std::vector<solution_t> solutions;
  unsigned int n = ceil(tsp.n / 2.0);

  if (heuristic == RANDOM) {
    return solve_random(tsp, n);
  }

  auto fn = heuristic_t_fn[heuristic];

  for (unsigned int i = 0; i < tsp.n; i++) {
    solutions.push_back(fn(tsp, n, i));
  }

  return solutions;
}
