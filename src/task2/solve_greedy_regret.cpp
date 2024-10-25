#include "../common/search.cpp"
#include "../common/types.cpp"

#include <algorithm>
#include <climits>
#include <numeric>
#include <utility>

#define REGRET_K 2
#define REGRET_WEIGHT 0.5

typedef std::pair<unsigned int, int> pos_cost_t;

solution_t solve_regret(const tsp_t &tsp, unsigned int n, unsigned int start,
                        float weight) {

    solution_t solution(tsp, find_cycle(tsp, start));

    std::vector<pos_cost_t> costs;
    costs.reserve(n);

    while (solution.path.size() < n) {
        std::optional<unsigned int> max;
        int max_score = INT_MIN;
        int idx = 0;

        for (auto node : solution.remaining_nodes) {
            costs.clear();

            for (int i = 0; i < solution.path.size(); i++) {
                costs.push_back({i, solution.get_cost_diff(node, i)});
            }

            std::sort(costs.begin(), costs.end(),
                      [](const pos_cost_t &pair1, const pos_cost_t &pair2) {
                          return pair1.second < pair2.second;
                      });

            int regret = std::accumulate(
                costs.begin(), costs.begin() + REGRET_K, 0,
                [&](int regret, pos_cost_t pair) {
                    return regret + pair.second - costs[0].second;
                });

            int score = weight * regret - (1 - weight) * costs[0].second;

            if (score > max_score) {
                max_score = score;
                max = node;
                idx = costs[0].first;
            }
        }

        solution.insert(max.value(), idx);
    }

    return solution;
}

solution_t solve_regret_weighted(const tsp_t &tsp, unsigned int n,
                                 unsigned int start) {
    return solve_regret(tsp, n, start, REGRET_WEIGHT);
}

solution_t solve_regret_unweighted(const tsp_t &tsp, unsigned int n,
                                   unsigned int start) {
    return solve_regret(tsp, n, start, 1.0);
}
