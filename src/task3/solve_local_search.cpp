#include <optional>

#include "../common/types.cpp"
#include "../common/utils.cpp"

enum IntrapathType {
    INTRA_NODE_SWAP,
    INTRA_EDGE_SWAP,
};

// Get all neighbors in an arbitrary order
std::vector<solution_t> get_all_neighbors(const solution_t &sol,
                                          IntrapathType intra_type) {
    std::set<unsigned> remaining_nodes(sol.path.begin(), sol.path.end());
    for (unsigned node = 0; node < sol.tsp->n; node++) {
        remaining_nodes.insert(node);
    }
    for (unsigned node : sol.path) {
        remaining_nodes.erase(node);
    }

    std::vector<solution_t> neighbors;
    neighbors.reserve(sol.tsp->n * sol.tsp->n);
    for (int i = 0; i < sol.path.size(); i++) {
        for (int j = i + 1; j < sol.path.size(); j++) {
            solution_t new_sol = sol;
            if (intra_type == INTRA_NODE_SWAP) {
                new_sol.swap_nodes(i, j);
            } else if (intra_type == INTRA_EDGE_SWAP) {
                new_sol.swap_edges(i, j);
            }
            neighbors.push_back(new_sol);
        }
        // Interpath operation (replace node)
        for (unsigned new_node : remaining_nodes) {
            solution_t new_sol = sol;
            new_sol.replace_node(i, new_node);
        }
    }
}

enum LocalSelectMethod { LOCAL_GREEDY, LOCAL_STEEPEST };

class LocalSearcher {
  private:
    std::optional<unsigned>
    select_greedy(const std::vector<solution_t> &all_neighbors,
                  const solution_t &curr_sol,
                  const std::vector<unsigned> &idxs) {
        for (unsigned idx : idxs) {
            if (all_neighbors[idx].cost < curr_sol.cost) {
                return idx;
            }
        }
        return;
    }

    std::optional<unsigned>
    select_steepest(const std::vector<solution_t> &all_neighbors,
                    const solution_t &curr_sol,
                    const std::vector<unsigned> &idxs) {
        int best_cost = curr_sol.cost;
        std::optional<unsigned> best_idx;
        for (unsigned idx : idxs) {
            if (all_neighbors[idx].cost < best_cost) {
                best_idx = idx;
                best_cost = all_neighbors[idx].cost;
            }
        }
        return best_idx;
    }

  public:
    const tsp_t &tsp;
    std::random_device rd;
    std::mt19937 g;
    LocalSelectMethod select_method;
    IntrapathType intrapath_type;

    LocalSearcher(const tsp_t &tsp, LocalSelectMethod select_method,
                  IntrapathType intrapath_type)
        : tsp(tsp), rd(), g(rd()), select_method(select_method),
          intrapath_type(intrapath_type) {}

    solution_t local_search(const solution_t &curr_sol,
                            LocalSelectMethod select_method) {
        solution_t best_sol = curr_sol;
        bool step_made = true;
        while (step_made) {
            step_made = false;
            auto all_neighbors = get_all_neighbors(curr_sol, intrapath_type);
            std::vector<unsigned> idxs(all_neighbors.size());
            std::shuffle(idxs.begin(), idxs.end(), g);

            std::optional<unsigned> select_res;
            if (select_method == LOCAL_GREEDY) {
                select_res = select_greedy(all_neighbors, curr_sol, idxs);
            } else if (select_method == LOCAL_STEEPEST) {
                select_res = select_steepest(all_neighbors, curr_sol, idxs);
            }

            if (select_res.has_value()) {
                step_made = true;
                best_sol = all_neighbors[select_res.value()];
            }
        }
        return best_sol;
    }
};