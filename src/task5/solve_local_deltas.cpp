#include <algorithm>
#include <chrono>
#include <optional>
#include <set>
#include <stack>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <vector>

#include "../common/types.cpp"
#include "../task1/solve_random.cpp"

struct edge_t {
    unsigned from;
    unsigned to;
};

struct UndirectedEdgeHash {
    std::size_t operator()(const edge_t &edge) const {
        unsigned a, b;
        if (edge.from < edge.to) {
            a = edge.from;
            b = edge.to;
        } else {
            a = edge.to;
            b = edge.from;
        }
        return std::hash<std::string>()(std::to_string(a) + "," +
                                        std::to_string(b));
    }
};

bool edges_same_direction(const edge_t &edge1, const edge_t &edge2) {
    if (edge1.from == edge2.from && edge1.to == edge2.to)
        return true;
    else if (edge1.from == edge2.to && edge1.to == edge2.from)
        return false;
    else
        throw std::logic_error("'edges_same_direction' shouldn't be called on "
                               "edges with differing nodes");
}

inline bool operator==(const edge_t &lhs, const edge_t &rhs) {
    return lhs.from == rhs.from && lhs.to == rhs.to;
}

struct oper_info_t {
    int delta;
    edge_t rem_edge1;                 // One of removed edges
    edge_t rem_edge2;                 // One of removed edges
    std::optional<unsigned> new_node; // If .has_value(), then the op is
                                      // REPLACE, otherwise - REVERSE

    inline bool is_intra() const { return !new_node.has_value(); }
};

inline bool operator==(const oper_info_t &lhs, const oper_info_t &rhs) {
    bool edges_equal =
        (lhs.rem_edge1 == rhs.rem_edge1 && lhs.rem_edge2 == rhs.rem_edge2) ||
        (lhs.rem_edge1 == rhs.rem_edge2 && lhs.rem_edge2 == rhs.rem_edge1);
    return lhs.delta == rhs.delta && edges_equal &&
           lhs.new_node == rhs.new_node;
}
inline bool operator!=(const oper_info_t &lhs, const oper_info_t &rhs) {
    return !(lhs == rhs);
}

// Comparator for `oper_pq_t`
class OperInfoCompare {
  public:
    bool operator()(const oper_info_t &left, const oper_info_t &right) const {
        if (left.delta == right.delta) {
            return left != right;
        }
        return left.delta < right.delta;
    }
};

// Operation priority queue
using oper_list_t = std::set<oper_info_t, OperInfoCompare>;

void update_list_with_new_opers(const solution_t &sol, oper_list_t &oper_list) {
    for (unsigned int i = 0; i < sol.path.size(); i++) {
        for (unsigned int j = i + 1; j < sol.path.size(); j++) {
            // REVERSE operation
            int op_delta = sol.reverse_delta(i, j);
            if (op_delta < 0)
                oper_list.insert(oper_info_t{
                    op_delta, edge_t{sol.path[sol.prev(i)], sol.path[i]},
                    edge_t{sol.path[j], sol.path[sol.next(j)]}, std::nullopt});
        }

        for (auto node : sol.remaining_nodes) {
            // REPLACE operation
            int op_delta = sol.replace_delta(node, i);
            if (op_delta < 0)
                oper_list.insert(oper_info_t{
                    op_delta, edge_t{sol.path[sol.prev(i)], sol.path[i]},
                    edge_t{sol.path[i], sol.path[sol.next(i)]}, node});
        }
    }
}

std::optional<operation_t> steepest_deltas_search(const solution_t &solution,
                                                  oper_list_t &oper_pq) {
    std::optional<oper_info_t> best_op_info;

    std::unordered_set<edge_t, UndirectedEdgeHash> edges_in_sol;
    for (unsigned i = 1; i < solution.path.size(); i++) {
        edges_in_sol.insert(
            edge_t{solution.path[solution.prev(i)], solution.path[i]});
    }
    edges_in_sol.insert(edge_t{solution.path.back(), solution.path.front()});

    // Main loop, iterating through move list
    std::stack<oper_list_t::iterator>
        iters_to_remove; // Stack is to start removing elems from the end
    for (auto iter = oper_pq.begin(); iter != oper_pq.end(); iter++) {
        auto edge_iter1 = edges_in_sol.find(iter->rem_edge1);
        auto edge_iter2 = edges_in_sol.find(iter->rem_edge2);
        if (edge_iter1 == edges_in_sol.end() ||
            edge_iter2 == edges_in_sol.end()) {
            iters_to_remove.push(iter);
            continue;
        }
        if (!edges_same_direction(*edge_iter1, iter->rem_edge1) ||
            !edges_same_direction(*edge_iter2, iter->rem_edge2))
            continue;

        best_op_info = *iter;
        break;
    }

    // Delete the moves in move list which are marked for deletion
    while (!iters_to_remove.empty()) {
        oper_pq.erase(iters_to_remove.top());
        iters_to_remove.pop();
    }

    if (!best_op_info.has_value()) {
        return std::nullopt;
    }

    if (best_op_info->is_intra()) {
        unsigned idx1 = std::find(solution.path.cbegin(), solution.path.cend(),
                                  best_op_info->rem_edge1.to) -
                        solution.path.cbegin(),
                 idx2 = std::find(solution.path.cbegin(), solution.path.cend(),
                                  best_op_info->rem_edge2.from) -
                        solution.path.cbegin();
        return operation_t{solution_t::REVERSE, idx1, idx2,
                           best_op_info->delta};
    } else {
        if (best_op_info->rem_edge1.to != best_op_info->rem_edge2.from)
            throw std::logic_error("Invalid order of edges in `edge_t`");
        unsigned pos = std::find(solution.path.cbegin(), solution.path.cend(),
                                 best_op_info->rem_edge1.to) -
                       solution.path.cbegin();
        return operation_t{solution_t::REPLACE, best_op_info->new_node.value(),
                           pos, best_op_info->delta};
    }
}

solution_t local_deltas_steepest(const tsp_t &tsp, solution_t solution) {
    oper_list_t oper_pq;

    while (true) {
        update_list_with_new_opers(solution, oper_pq);
        std::optional<operation_t> best_op =
            steepest_deltas_search(solution, oper_pq);

        solution.search_iters++;

        if (!best_op.has_value()) {
            break;
        }

        switch (best_op->type) {
        case solution_t::REVERSE:
            solution.reverse(best_op->arg1, best_op->arg2);
            break;
        case solution_t::REPLACE:
            solution.replace(best_op->arg1, best_op->arg2);
            break;
        default:
            throw std::logic_error("Invalid operation type");
            break;
        }
        if (!solution.is_valid()) {
            throw std::logic_error("Solution is invalid");
        } else if (!solution.is_cost_correct()) {
            throw std::logic_error("Solution cost is incorrect");
        }
    }

    return solution;
}

std::vector<solution_t> solve_local_deltas_steepest_random(const tsp_t &tsp,
                                                           unsigned n) {
    std::vector<solution_t> solutions = solve_random(tsp, n);
    for (int i = 0; i < solutions.size(); i++) {
        const auto start = std::chrono::high_resolution_clock().now();
        solutions[i] = local_deltas_steepest(tsp, solutions[i]);
        solutions[i].runtime_ms +=
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock().now() - start)
                .count();
    }
    return solutions;
}
