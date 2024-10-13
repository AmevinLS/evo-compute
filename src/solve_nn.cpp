#include "types.cpp"
#include <algorithm>
#include <climits>
#include <vector>

unsigned int find_nn(const tsp_t &tsp, const std::vector<unsigned int> &indices,
                     const solution_t &solution) {
  unsigned int current = solution.path.back();
  unsigned int min = current;
  int min_dist = INT_MAX;

  for (auto idx : indices) {
    if (idx == current) {
      continue;
    }

    if (std::find(solution.path.begin(), solution.path.end(), idx) !=
        solution.path.end()) {
      continue;
    }

    int dist = tsp.adj_matrix(current, idx);

    if (dist < min_dist) {
      min = idx;
      min_dist = dist;
    }
  }

  return min;
}

solution_t solve_nn_end(const tsp_t &tsp,
                        const std::vector<unsigned int> &indices) {
  solution_t solution(tsp.nodes[indices[0]].weight, {indices[0]});

  do {
    unsigned int min = find_nn(tsp, indices, solution);
    solution.cost +=
        tsp.adj_matrix(solution.path.back(), min) + tsp.nodes[min].weight;
    solution.path.push_back(min);
  } while (solution.path.size() < indices.size());

  solution.cost += tsp.adj_matrix(solution.path.back(), solution.path.front());
  return solution;
}

solution_t solve_nn_any(const tsp_t &tsp,
                        const std::vector<unsigned int> &indices) {
  solution_t solution(tsp.nodes[indices[0]].weight, {indices[0]});

  do {
    unsigned int min = find_nn(tsp, indices, solution);
    solution.cost += tsp.nodes[min].weight;

    int cost_best = solution.cost + tsp.adj_matrix(min, solution.path.front());
    int idx = 0;

    for (int i = 0; i < solution.path.size() - 1; i++) {
      unsigned int a = solution.path[i];
      unsigned int b = solution.path[i + 1];

      int cost_new = solution.cost - tsp.adj_matrix(a, b) +
                     tsp.adj_matrix(a, min) + tsp.adj_matrix(min, b);
      if (cost_new < cost_best) {
        cost_best = cost_new;
        idx = i + 1;
      }
    }

    int cost_new = solution.cost + tsp.adj_matrix(solution.path.back(), min);

    if (cost_new < cost_best) {
      solution.path.push_back(min);
      solution.cost = cost_new;
    } else {
      solution.path.insert(solution.path.begin() + idx, min);
      solution.cost = cost_best;
    }

  } while (solution.path.size() < indices.size());
  solution.cost += tsp.adj_matrix(solution.path.back(), solution.path.front());

  return solution;
}
