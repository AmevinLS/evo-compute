#include <algorithm>
#include <iostream>
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

    operation_t(std::function<void(solution_t &)> func, int cost_diff)
        : func(func), cost_diff(cost_diff) {}
};

class OperProducer {
  private:
    std::set<unsigned> free_nodes;

  public:
    solution_t &sol;
    IntrapathType intra_type;

    OperProducer(solution_t &sol, IntrapathType intra_type)
        : sol(sol), intra_type(intra_type) {
        free_nodes = std::set<unsigned>();
        for (unsigned i = 0; i < sol.tsp->n; i++) {
            free_nodes.insert(i);
        }
        for (unsigned i : sol.path) {
            free_nodes.erase(i);
        }
    }

    unsigned get_num_operations() {
        unsigned path_size = sol.path.size();
        return path_size * (path_size - 1) / 2 +
               path_size * (sol.tsp->n - path_size);
    }

    std::vector<operation_t> get_all_operations() {
        unsigned path_size = sol.path.size();
        unsigned num_opers = get_num_operations();
        std::vector<operation_t> operations;
        operations.reserve(num_opers);
        for (unsigned i = 0; i < path_size; i++) {
            for (unsigned j = i + 1; j < path_size; j++) {
                if (intra_type == INTRA_NODE_SWAP) {
                    operations.emplace_back(
                        [i, j](solution_t &sol) {
                            // std::cout << "Selected NODE_SWAP" << "\n";
                            sol.swap_nodes(i, j);
                        },
                        sol.swap_nodes_cost_diff(i, j));
                } else if (intra_type == INTRA_EDGE_SWAP) {
                    operations.emplace_back(
                        [i, j](solution_t &sol) {
                            // std::cout << "Selected EDGE_SWAP" << "\n";
                            sol.swap_edges(i, j);
                        },
                        sol.swap_edges_cost_diff(i, j));
                }
            }
            // Interpath operation (replace node)
            for (unsigned new_node : free_nodes) {
                operations.emplace_back(
                    [i, new_node, this](solution_t &sol) {
                        // std::cout << "Selected NODE_REPLACE" << "\n";
                        this->free_nodes.insert(sol.path[i]);
                        this->free_nodes.erase(new_node);
                        sol.replace_node(i, new_node);
                    },
                    sol.replace_node_cost_diff(i, new_node));
            }
        }
        return std::move(operations);
    }
};

// Get all neighbors in an arbitrary order
// std::vector<operation_t> get_all_operations(const solution_t &sol,
//                                             IntrapathType intra_type) {
//     std::set<unsigned> remaining_nodes;
//     for (unsigned node = 0; node < sol.tsp->n; node++) {
//         remaining_nodes.insert(node);
//     }
//     for (unsigned node : sol.path) {
//         remaining_nodes.erase(node);
//     }

//     std::vector<operation_t> operations;
//     operations.reserve(sol.tsp->n * sol.tsp->n);
//     for (int i = 0; i < sol.path.size(); i++) {
//         for (int j = i + 1; j < sol.path.size(); j++) {
//             operation_t operation;
//             if (intra_type == INTRA_NODE_SWAP) {
//                 operation.cost_diff = sol.swap_nodes_cost_diff(i, j);
//                 operation.func = [i, j](solution_t &sol) {
//                     sol.swap_nodes(i, j);
//                 };
//             } else if (intra_type == INTRA_EDGE_SWAP) {
//                 operation.cost_diff = sol.swap_edges_cost_diff(i, j);
//                 operation.func = [i, j](solution_t &sol) {
//                     sol.swap_edges(i, j);
//                 };
//             }
//             operations.push_back(operation);
//         }
//         // Interpath operation (replace node)
//         for (unsigned new_node : remaining_nodes) {
//             operation_t operation;
//             solution_t new_sol = sol;
//             operation.cost_diff = sol.replace_node_cost_diff(i, new_node);
//             operation.func = [i, new_node](solution_t &sol) {
//                 sol.replace_node(i, new_node);
//             };
//             operations.push_back(operation);
//         }
//     }
//     return operations;
// }

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
        bool step_made = true;
        solution_t best_sol = curr_sol;
        OperProducer oper_producer(best_sol, intrapath_type);
        std::vector<unsigned> idxs(oper_producer.get_num_operations());
        for (unsigned i = 0; i < idxs.size(); i++) {
            idxs[i] = i;
        }

        while (step_made) {
            step_made = false;
            std::vector<operation_t> all_operations =
                oper_producer.get_all_operations();

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
                // std::cout << "Cost after step: " << best_sol.cost << "\n";
            }

            // std::set<unsigned> unique_nodes(best_sol.path.begin(),
            //                                 best_sol.path.end());
            // if (unique_nodes.size() < best_sol.path.size()) {
            //     throw std::logic_error("Nodes are repeated in path");
            // }
        }
        return best_sol;
    }
};