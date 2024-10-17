#include "../common/types.cpp"
#include <algorithm>
#include <climits>
#include <vector>

std::pair<unsigned int, int>
find_nn(const tsp_t &tsp, const solution_t &solution, unsigned int current) {
  unsigned int min = current;
  int min_dist = INT_MAX;

  for (int idx = 0; idx < tsp.n; idx++) {
    if (idx == current) {
      continue;
    }

    if (std::find(solution.path.begin(), solution.path.end(), idx) !=
        solution.path.end()) {
      continue;
    }

    int dist = tsp.adj_matrix(current, idx) + tsp.nodes[idx].weight;

    if (dist < min_dist) {
      min = idx;
      min_dist = dist;
    }
  }

  return {min, min_dist};
}

solution_t solve_nn_end(const tsp_t &tsp, unsigned int n, unsigned int start) {
  solution_t solution(tsp.nodes[start].weight, {start});

  do {
    auto [min, dist] = find_nn(tsp, solution, solution.path.back());

    solution.cost += dist;
    solution.path.push_back(min);
  } while (solution.path.size() < n);

  solution.cost += tsp.adj_matrix(solution.path.back(), solution.path.front());
  return solution;
}

solution_t solve_nn_any(const tsp_t &tsp, unsigned int n, unsigned int start) {
  solution_t solution(tsp.nodes[start].weight, {start});

  do {
    auto [min, min_dist] = find_nn(tsp, solution, solution.path.front());
    unsigned int idx = 0;

    for (int i = 0; i < solution.path.size() - 1; i++) {
      unsigned int a = solution.path[i];
      unsigned int b = solution.path[i + 1];

      for (int j = 0; j < tsp.n; j++) {
        if (j == a || j == b ||
            std::find(solution.path.begin(), solution.path.end(), j) !=
                solution.path.end()) {

          continue;
        }
        int dist = tsp.nodes[j].weight + tsp.adj_matrix(a, j) +
                   tsp.adj_matrix(j, b) - tsp.adj_matrix(a, b);
        if (dist < min_dist) {
          min = j;
          min_dist = dist;
          idx = i + 1;
        }
      }
    }

    auto [last_min, dist] = find_nn(tsp, solution, solution.path.back());
    if (dist < min_dist) {
      solution.path.push_back(last_min);
      min_dist = dist;
    } else {
      solution.path.insert(solution.path.begin() + idx, min);
    }

    solution.cost += min_dist;
  } while (solution.path.size() < n);

  solution.cost += tsp.adj_matrix(solution.path.back(), solution.path.front());

  return solution;
}
