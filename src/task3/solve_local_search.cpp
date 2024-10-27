#include <algorithm>
#include <iterator>
#include <map>
#include <optional>
#include <random>
#include <vector>

#include "../common/types.cpp"

std::random_device rd;
std::mt19937 gen;

solution_t random_solution(const tsp_t &tsp, unsigned sol_size) {
    std::vector<unsigned> path(sol_size);
    for (int i = 0; i < path.size(); i++) {
        path[i] = i;
    }
    std::shuffle(path.begin(), path.end(), gen);
    return solution_t(tsp, path);
}

int random_int(int low, int high) {
    std::uniform_int_distribution distr(low, high - 1);
    return distr(gen);
}

enum IntrapathType {
    INTRA_NODE_SWAP,
    INTRA_EDGE_SWAP,
};

struct operation_t {
    std::function<void(solution_t &)> func;
    int cost_diff;
};

// class SolutionTracker {
//   private:
//     std::vector<operation_t> all_opers;
//     std::map<std::pair<unsigned, unsigned>, operation_t> node_swap_opers;
//     std::map<std::pair<unsigned, unsigned>, operation_t> edge_swap_opers;
//     std::map<std::pair<unsigned, unsigned>, operation_t>
//         node_replace_opers; // pair is (idx, new_node)

//     std::vector<operation_t> get_all_operations() const {
//         std::set<unsigned> remaining_nodes(sol.path.begin(), sol.path.end());
//         for (unsigned node = 0; node < sol.tsp->n; node++) {
//             remaining_nodes.insert(node);
//         }
//         for (unsigned node : sol.path) {
//             remaining_nodes.erase(node);
//         }

//         std::vector<operation_t> operations;
//         operations.reserve(sol.tsp->n * sol.tsp->n);
//         for (int i = 0; i < sol.path.size(); i++) {
//             for (int j = i + 1; j < sol.path.size(); j++) {
//                 operation_t operation;
//                 int cost_diff;
//                 if (intra_type == INTRA_NODE_SWAP) {
//                     operation.cost_diff = sol.swap_nodes_cost_diff(i, j);
//                     operation.func = [i, j](solution_t &sol) {
//                         sol.swap_nodes(i, j);
//                     };
//                 } else if (intra_type == INTRA_EDGE_SWAP) {
//                     operation.cost_diff = sol.swap_edges_cost_diff(i, j);
//                     operation.func = [i, j](solution_t &sol) {
//                         sol.swap_edges(i, j);
//                     };
//                 }
//                 operations.push_back(operation);
//             }
//             // Interpath operation (replace node)
//             for (unsigned new_node : remaining_nodes) {
//                 operation_t operation;
//                 solution_t new_sol = sol;
//                 operation.cost_diff = sol.replace_node_cost_diff(i,
//                 new_node); operation.func = [i, new_node](solution_t &sol) {
//                     sol.replace_node(i, new_node);
//                 };
//                 operations.push_back(operation);
//             }
//         }
//         return operations;
//     }

//   public:
//     solution_t &sol;
//     std::vector<operation_t> operations;
//     IntrapathType intra_type;

//     SolutionTracker(solution_t &sol, IntrapathType intra_type)
//         : sol(sol), intra_type(intra_type), operations(get_all_operations())
//         {}

//     unsigned get_total_opers() const {
//         return node_swap_opers.size() + edge_swap_opers.size() +
//                node_replace_opers.size();
//     }

//     void perform_oper(unsigned oper_idx) {
//         unsigned node_swap_barrier = node_swap_opers.size();
//         unsigned edge_swap_barrier = node_swap_barrier +
//         edge_swap_opers.size(); unsigned node_replace_barrier =
//             edge_swap_barrier + node_replace_opers.size();
//         if (oper_idx < node_replace_barrier) {
//             auto item_iter = std::advance(node_replace_opers.begin(), 5);

//         } else if (oper_idx < edge_swap_barrier) {
//             oper_idx -= node_replace_barrier;

//         } else if (oper_idx < node_replace_barrier) {
//             oper_idx -= edge_swap_barrier;
//         }
//     }
// };

// Get all neighbors in an arbitrary order
std::vector<operation_t> get_all_operations(const solution_t &sol,
                                            IntrapathType intra_type) {
    std::set<unsigned> remaining_nodes;
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

    LocalSearcher(const tsp_t &tsp) : tsp(tsp) {}

    solution_t local_search(const solution_t &curr_sol,
                            LocalSelectMethod select_method,
                            IntrapathType intrapath_type) {
        solution_t best_sol = curr_sol;
        bool step_made = true;
        while (step_made) {
            step_made = false;
            std::vector all_operations =
                get_all_operations(best_sol, intrapath_type);
            std::vector<unsigned> idxs(all_operations.size());
            for (unsigned i = 0; i < idxs.size(); i++) {
                idxs[i] = i;
            }
            std::shuffle(idxs.begin(), idxs.end(), gen);

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