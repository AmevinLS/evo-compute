#include <optional>
#include <random>
#include <vector>

#include "../common/types.cpp"

solution_t random_solution(const tsp_t &tsp, unsigned sol_size) {
    std::vector<unsigned> path(sol_size);
    for (int i = 0; i < path.size(); i++) {
        path[i] = i;
    }
    return solution_t(tsp, path);
}

enum IntrapathType {
    INTRA_NODE_SWAP,
    INTRA_EDGE_SWAP,
};

struct operation_t {
    std::function<void(solution_t &)> func;
    int cost_diff;
};

// Get all neighbors in an arbitrary order
std::vector<operation_t> get_all_operations(const solution_t &sol,
                                            IntrapathType intra_type) {
    std::set<unsigned> remaining_nodes(sol.path.begin(), sol.path.end());
    for (unsigned node = 0; node < sol.tsp->n; node++) {
        remaining_nodes.insert(node);
    }
    for (unsigned node : sol.path) {
        remaining_nodes.erase(node);
    }

    std::vector<operation_t> operations;
    operations.reserve(sol.tsp->n * sol.tsp->n);
    for (int i = 0; i < sol.path.size(); i++) {
        for (int j = i + 1; j < sol.path.size(); j++) {
            operation_t operation;
            int cost_diff;
            if (intra_type == INTRA_NODE_SWAP) {
                operation.cost_diff = sol.swap_nodes_cost_diff(i, j);
                operation.func = [i, j](solution_t &sol) {
                    sol.swap_nodes(i, j);
                };
            } else if (intra_type == INTRA_EDGE_SWAP) {
                operation.cost_diff = sol.swap_edges_cost_diff(i, j);
                operation.func = [i, j](solution_t &sol) {
                    sol.swap_edges(i, j);
                };
            }
            operations.push_back(operation);
        }
        // Interpath operation (replace node)
        for (unsigned new_node : remaining_nodes) {
            operation_t operation;
            solution_t new_sol = sol;
            operation.cost_diff = sol.replace_node_cost_diff(i, new_node);
            operation.func = [i, new_node](solution_t &sol) {
                sol.replace_node(i, new_node);
            };
            operations.push_back(operation);
        }
    }
    return operations;
}

enum LocalSelectMethod { LOCAL_GREEDY, LOCAL_STEEPEST };

class LocalSearcher {
  private:
    std::optional<unsigned>
    select_greedy(const std::vector<operation_t> &all_operations,
                  const std::vector<unsigned> &idxs) {
        for (unsigned idx : idxs) {
            if (all_operations[idx].cost_diff < 0) {
                return idx;
            }
        }
        return std::nullopt;
    }

    std::optional<unsigned>
    select_steepest(const std::vector<operation_t> &all_operations,
                    const std::vector<unsigned> &idxs) {
        int best_diff = 0;
        std::optional<unsigned> best_idx;
        for (unsigned idx : idxs) {
            if (all_operations[idx].cost_diff < best_diff) {
                best_idx = idx;
                best_diff = all_operations[idx].cost_diff;
            }
        }
        return best_idx;
    }

  public:
    const tsp_t &tsp;
    std::random_device rd;
    std::mt19937 g;

    LocalSearcher(const tsp_t &tsp) : tsp(tsp), rd(), g(rd()) {}

    solution_t local_search(const solution_t &curr_sol,
                            LocalSelectMethod select_method,
                            IntrapathType intrapath_type) {
        solution_t best_sol = curr_sol;
        bool step_made = true;
        while (step_made) {
            step_made = false;
            std::vector all_operations =
                get_all_operations(curr_sol, intrapath_type);
            std::vector<unsigned> idxs(all_operations.size());
            for (unsigned i = 0; i < idxs.size(); i++) {
                idxs[i] = i;
            }
            std::shuffle(idxs.begin(), idxs.end(), g);

            std::optional<unsigned> select_res;
            if (select_method == LOCAL_GREEDY) {
                select_res = select_greedy(all_operations, idxs);
            } else if (select_method == LOCAL_STEEPEST) {
                select_res = select_steepest(all_operations, idxs);
            }

            if (select_res.has_value()) {
                step_made = true;
                all_operations[select_res.value()].func(best_sol);
                std::cout << "Cost after step: " << best_sol.cost << "\n";
            }
        }
        return best_sol;
    }
};