#include <algorithm>
#include <cassert>
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

inline bool operator==(const edge_t &lhs, const edge_t &rhs) {
    return lhs.from == rhs.from && lhs.to == rhs.to;
}

struct oper_info_t {
    int delta;
    edge_t rem_edge1;                 // One of removed edges
    edge_t rem_edge2;                 // One of removed edges
    std::optional<unsigned> new_node; // If .has_value(), then the op is
                                      // REPLACE, otherwise - REVERSE
};

inline bool operator==(const oper_info_t &lhs, const oper_info_t &rhs) {
    bool edges_equal =
        (lhs.rem_edge1 == rhs.rem_edge1 && lhs.rem_edge2 == rhs.rem_edge2) ||
        (lhs.rem_edge1 == rhs.rem_edge2 && lhs.rem_edge2 == rhs.rem_edge1);
    return lhs.delta == rhs.delta && lhs.new_node == rhs.new_node &&
           edges_equal;
}
inline bool operator!=(const oper_info_t &lhs, const oper_info_t &rhs) {
    return !(lhs == rhs);
}

inline oper_info_t get_reverse_op_info(const solution_t &sol, unsigned idx1,
                                       unsigned idx2, int delta) {
    return oper_info_t{delta, edge_t{sol.path[sol.prev(idx1)], sol.path[idx1]},
                       edge_t{sol.path[idx2], sol.path[sol.next(idx2)]},
                       std::nullopt};
}

inline oper_info_t get_replace_op_info(const solution_t &sol, unsigned node,
                                       unsigned pos, int delta) {
    return oper_info_t{delta, edge_t{sol.path[sol.prev(pos)], sol.path[pos]},
                       edge_t{sol.path[pos], sol.path[sol.next(pos)]}, node};
}

inline std::pair<unsigned, unsigned>
swap_edges_to_reverse_args(const solution_t &sol, unsigned edge_idx1,
                           unsigned edge_idx2) {
    if (edge_idx1 == edge_idx2)
        throw std::invalid_argument("Cannot swap an edge with itself");
    if (edge_idx1 > edge_idx2)
        std::swap(edge_idx1, edge_idx2);
    if (edge_idx2 - edge_idx1 == 1)
        throw std::invalid_argument("Should not swap neighboring edges");

    return {sol.next(edge_idx1), edge_idx2};
}

// Operation priority queue
struct oper_queue_t {
    // Comparator for `oper_pq_t`
    class OperInfoCompare {
      public:
        inline bool operator()(const oper_info_t &left,
                               const oper_info_t &right) const {
            if (left.delta == right.delta) {
                return left != right;
            }
            return left.delta < right.delta;
        }
    };
    using oper_list_t = std::set<oper_info_t, OperInfoCompare>;

    oper_list_t oper_list;

    oper_queue_t(const solution_t &sol) : oper_list() {
        for (unsigned int i = 0; i < sol.path.size(); i++) {
            for (unsigned int j = i + 1; j < sol.path.size(); j++) {
                // REVERSE operation
                int op_delta = sol.reverse_delta(i, j);
                if (op_delta < 0)
                    oper_list.emplace(get_reverse_op_info(sol, i, j, op_delta));
            }

            for (unsigned node : sol.remaining_nodes) {
                // REPLACE operation
                int op_delta = sol.replace_delta(node, i);
                if (op_delta < 0)
                    oper_list.emplace(
                        get_replace_op_info(sol, node, i, op_delta));
            }
        }
    }

    void update_from_new_edge(const solution_t &new_sol,
                              unsigned new_edge_idx) {
        unsigned stop_idx = new_sol.prev(new_edge_idx);
        for (unsigned edge_idx = new_sol.next(new_sol.next(new_edge_idx));
             edge_idx != stop_idx; edge_idx = new_sol.next(edge_idx)) {
            auto [idx1, idx2] =
                swap_edges_to_reverse_args(new_sol, new_edge_idx, edge_idx);
            int delta = new_sol.reverse_delta(idx1, idx2);
            if (delta < 0)
                oper_list.emplace(
                    get_reverse_op_info(new_sol, idx1, idx2, delta));
        }

        for (unsigned node : new_sol.remaining_nodes) {
            for (unsigned idx : {new_edge_idx, new_sol.next(new_edge_idx)}) {
                int delta = new_sol.replace_delta(node, idx);
                if (delta < 0)
                    oper_list.emplace(
                        get_replace_op_info(new_sol, node, idx, delta));
            }
        }
    }

    void update_with_oper(const solution_t &new_sol,
                          const operation_t &last_oper,
                          std::optional<unsigned> removed_node = std::nullopt) {
        if (last_oper.type == solution_t::REVERSE) {
            for (unsigned edge_idx = new_sol.prev(last_oper.arg1);
                 edge_idx != new_sol.next(last_oper.arg2);
                 edge_idx = new_sol.next(edge_idx))
                update_from_new_edge(new_sol, edge_idx);
        } else if (last_oper.type == solution_t::REPLACE) {
            assert(removed_node.has_value());
            update_from_new_edge(new_sol, new_sol.prev(last_oper.arg2));
            update_from_new_edge(new_sol, last_oper.arg2);
            for (unsigned pos = 0; pos < new_sol.path.size(); pos++) {
                int delta = new_sol.replace_delta(removed_node.value(), pos);
                if (delta < 0)
                    oper_list.emplace(get_replace_op_info(
                        new_sol, removed_node.value(), pos, delta));
            }
        } else {
            throw std::logic_error("Unpermitted operation happened");
        }
    }
};

inline std::optional<unsigned> get_node_idx(const solution_t &sol,
                                            unsigned node) {
    auto iter = std::find(sol.path.cbegin(), sol.path.cend(), node);
    if (iter == sol.path.cend()) {
        return std::nullopt;
    }
    return iter - sol.path.cbegin();
}

struct edge_tracker_t {
    struct UndirectedEdgeHash {
        inline std::size_t operator()(const edge_t &edge) const {
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

    enum verdict_t { REMOVE, LEAVE, USE };

    std::unordered_set<edge_t, UndirectedEdgeHash> edges_in_sol;

    inline edge_tracker_t(const solution_t &solution)
        : edges_in_sol(solution_to_edges(solution)) {}

    inline static std::unordered_set<edge_t, UndirectedEdgeHash>
    solution_to_edges(const solution_t &solution) {
        std::unordered_set<edge_t, UndirectedEdgeHash> edge_set;
        edge_set.reserve(solution.path.size());
        for (unsigned i = 1; i < solution.path.size(); i++) {
            edge_set.emplace(
                edge_t{solution.path[solution.prev(i)], solution.path[i]});
        }
        edge_set.emplace(edge_t{solution.path.back(), solution.path.front()});
        return edge_set;
    }

    inline void update(const solution_t &solution) {
        edges_in_sol = solution_to_edges(solution);

        // edges_in_sol.erase(oper_info.rem_edge1);
        // edges_in_sol.erase(oper_info.rem_edge2);

        // if (oper_info.new_node.has_value()) {
        //     edges_in_sol.insert(
        //         edge_t{oper_info.rem_edge1.from,
        //         oper_info.new_node.value()});
        //     edges_in_sol.insert(
        //         edge_t{oper_info.new_node.value(), oper_info.rem_edge2.to});
        // } else {
        //     edges_in_sol.insert(
        //         edge_t{oper_info.rem_edge1.from, oper_info.rem_edge2.from});
        //     edges_in_sol.insert(
        //         edge_t{oper_info.rem_edge1.to, oper_info.rem_edge2.to});
        // }
        // if (edges_in_sol.size() != solution.path.size())
        //     throw std::logic_error("Number of edges different than path
        //     size");
    }

    inline static bool edges_same_direction(const edge_t &edge1,
                                            const edge_t &edge2) {
        if (edge1.from == edge2.from && edge1.to == edge2.to)
            return true;
        else if (edge1.from == edge2.to && edge1.to == edge2.from)
            return false;
        else
            throw std::logic_error(
                "'edges_same_direction' shouldn't be called on "
                "edges with differing nodes");
    }

    verdict_t check(const solution_t &sol, const oper_info_t &oper_info) const {
        // Handle case when new_node already in solution
        if (oper_info.new_node.has_value() &&
            sol.remaining_nodes.count(oper_info.new_node.value()) == 0)
            return REMOVE;

        auto edge_iter1 = edges_in_sol.find(oper_info.rem_edge1);
        auto edge_iter2 = edges_in_sol.find(oper_info.rem_edge2);
        // Handle case if at least on of defining edges not present
        if (edge_iter1 == edges_in_sol.end() ||
            edge_iter2 == edges_in_sol.end()) {
            return REMOVE;
        }

        // Handle case if at least one is in the wrong direction
        if (!edges_same_direction(*edge_iter1, oper_info.rem_edge1) ||
            !edges_same_direction(*edge_iter2, oper_info.rem_edge2))
            return LEAVE;

        // Handle case 1->2->3, and we want to swap 1->2 and 2->3
        if (!oper_info.new_node.has_value() &&
            (get_node_idx(sol, edge_iter1->to) ==
                 get_node_idx(sol, edge_iter2->from) ||
             get_node_idx(sol, edge_iter1->from) ==
                 get_node_idx(sol, edge_iter2->to)))
            return LEAVE;

        return USE;
    }
};

operation_t convert_info_to_oper(const solution_t &solution,
                                 const oper_info_t &oper_info) {
    if (!oper_info.new_node.has_value()) {
        // This is a REVERSE operation
        unsigned idx1 = get_node_idx(solution, oper_info.rem_edge1.to).value(),
                 idx2 =
                     get_node_idx(solution, oper_info.rem_edge2.from).value();
        if (idx1 > idx2) {
            std::swap(idx1, idx2);
            idx1 = solution.next(idx1);
            idx2 = solution.prev(idx2);
        }
        return operation_t{solution_t::REVERSE, idx1, idx2, oper_info.delta};
    } else {
        // This is a REPLACE operation
        if (oper_info.rem_edge1.to != oper_info.rem_edge2.from)
            throw std::logic_error("Invalid order of edges in `edge_t`");
        unsigned pos = std::find(solution.path.cbegin(), solution.path.cend(),
                                 oper_info.rem_edge1.to) -
                       solution.path.cbegin();
        return operation_t{solution_t::REPLACE, oper_info.new_node.value(), pos,
                           oper_info.delta};
    }
}

std::optional<operation_t>
steepest_deltas_search(const solution_t &solution, oper_queue_t &oper_pq,
                       edge_tracker_t &edge_tracker) {
    // Main loop, iterating through move list
    std::optional<oper_info_t> best_op_info;
    std::stack<oper_queue_t::oper_list_t::iterator>
        iters_to_remove; // Stack is to start removing elems from the end
    for (auto iter = oper_pq.oper_list.begin(); iter != oper_pq.oper_list.end();
         iter++) {
        edge_tracker_t::verdict_t verdict = edge_tracker.check(solution, *iter);
        if (verdict == edge_tracker_t::REMOVE) {
            iters_to_remove.push(iter);
            continue;
        } else if (verdict == edge_tracker_t::LEAVE) {
            continue;
        } else if (verdict == edge_tracker_t::USE) {
            best_op_info = *iter;
            iters_to_remove.push(iter);
            break;
        }
    }

    // Delete the moves in move list which are marked for deletion
    while (!iters_to_remove.empty()) {
        oper_pq.oper_list.erase(iters_to_remove.top());
        iters_to_remove.pop();
    }

    if (!best_op_info.has_value()) {
        return std::nullopt;
    }
    return convert_info_to_oper(solution, best_op_info.value());
}

solution_t local_deltas_steepest(const tsp_t &tsp, solution_t solution) {
    oper_queue_t oper_pq(solution);
    edge_tracker_t edge_tracker(solution);

    while (true) {
        std::optional<operation_t> best_op =
            steepest_deltas_search(solution, oper_pq, edge_tracker);

        solution.search_iters++;

        if (!best_op.has_value()) {
            break;
        }

        switch (best_op->type) {
        case solution_t::REVERSE:
            solution.reverse(best_op->arg1, best_op->arg2);
            oper_pq.update_with_oper(solution, best_op.value());
            break;
        case solution_t::REPLACE: {
            unsigned removed_node = solution.path[best_op->arg2];
            solution.replace(best_op->arg1, best_op->arg2);
            oper_pq.update_with_oper(solution, best_op.value(), removed_node);
            break;
        }
        default:
            throw std::logic_error("Invalid operation type");
            break;
        }
        edge_tracker.update(solution);
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
