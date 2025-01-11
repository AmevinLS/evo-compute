#pragma once

#include <algorithm>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "../common/random.cpp"
#include "../common/types.cpp"
#include "../task2/solve_greedy_regret.cpp"

struct Edge {
    unsigned from;
    unsigned to;
};

inline bool operator==(const Edge &lhs, const Edge &rhs) {
    return lhs.from == rhs.from && lhs.to == rhs.to;
}

struct UndirectedEdgeHash {
    inline std::size_t operator()(const Edge &edge) const {
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

std::unordered_set<Edge, UndirectedEdgeHash>
SolutionToEdges(const solution_t &solution) {
    std::unordered_set<Edge, UndirectedEdgeHash> edge_set;
    edge_set.reserve(solution.path.size());
    for (unsigned i = 1; i < solution.path.size(); i++) {
        edge_set.emplace(
            Edge{solution.path[solution.prev(i)], solution.path[i]});
    }
    edge_set.emplace(Edge{solution.path.back(), solution.path.front()});
    return edge_set;
}

std::vector<unsigned> BuildPathFromCommon(const tsp_t &tsp,
                                          const solution_t &sol1,
                                          const solution_t &sol2,
                                          bool fill_missing) {
    auto edge_set = SolutionToEdges(sol2);
    std::vector<bool> is_node_removed(sol1.path.size(), true);
    std::vector<unsigned> new_path;
    std::set<unsigned> new_path_node_set;
    bool prev_edge_good = false;
    for (unsigned i = 0; i < sol1.path.size(); i++) {
        Edge edge = Edge{sol1.path[i], sol1.path[sol1.next(i)]};
        bool curr_edge_good = (edge_set.count(edge) > 0);
        if (prev_edge_good || curr_edge_good) {
            new_path.push_back(edge.from);
            new_path_node_set.insert(edge.from);
            is_node_removed[i] = false;
        } else {
            if (fill_missing)
                new_path.push_back(0);
        }
        prev_edge_good = curr_edge_good;
    }

    if (fill_missing) {
        std::vector<unsigned> remaining_nodes;
        for (unsigned i = 0; i < tsp.n; i++) {
            if (new_path_node_set.count(i) == 0)
                remaining_nodes.push_back(i);
        }
        auto rd = std::random_device();
        auto rng = std::default_random_engine(rd());
        std::shuffle(remaining_nodes.begin(), remaining_nodes.end(), rng);

        unsigned remaining_idx = 0;
        for (unsigned i = 0; i < new_path.size(); i++) {
            if (!is_node_removed[i])
                continue;
            new_path[i] = remaining_nodes[remaining_idx++];
        }
    }

    // if (new_path.size() != sol1.path.size()) {
    //     throw std::logic_error("Something is baad");
    // }

    return new_path;
}

class RecombOper {
  public:
    RecombOper(const tsp_t &tsp) : tsp_(tsp) {}

    virtual solution_t Recombine(const solution_t &sol1,
                                 const solution_t &sol2) const = 0;

    virtual std::string GetName() const = 0;

    virtual ~RecombOper() {}

    const tsp_t &tsp_;
};

class RandomFillOper : public RecombOper {
  public:
    using RecombOper::RecombOper;

    solution_t Recombine(const solution_t &sol1, const solution_t &sol2) const {
        return solution_t(tsp_, BuildPathFromCommon(tsp_, sol1, sol2, true));
    };

    std::string GetName() const { return "RandomFill"; }
};

class HeuristicRepairOper : public RecombOper {
  public:
    using RecombOper::RecombOper;

    solution_t Recombine(const solution_t &sol1, const solution_t &sol2) const {
        std::vector<unsigned> new_path =
            BuildPathFromCommon(tsp_, sol1, sol2, false);
        if (new_path.size() == 0) {
            new_path = find_cycle(tsp_, random_num(0, 200));
        } else if (new_path.size() < 3) {
            unsigned start_node = new_path[random_num(0, new_path.size())];
            new_path = find_cycle(tsp_, start_node);
        }
        solution_t res_sol(tsp_, new_path);
        res_sol = solve_regret(res_sol, 100, 0.5);
        return res_sol;
    }

    std::string GetName() const { return "HeuristicRepair"; }
};