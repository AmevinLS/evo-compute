#pragma once

#include <algorithm>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <vector>

struct node_t {
    int x;
    int y;
    unsigned int weight;
};

struct edge_t {
    unsigned int from;
    unsigned int to;
};

inline bool operator==(const edge_t &lhs, const edge_t &rhs) {
    return lhs.from == rhs.from && lhs.to == rhs.to;
}

template <> struct std::hash<edge_t> {
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

typedef std::vector<unsigned int> list_t;
typedef std::vector<list_t> matrix_t;

struct adj_matrix_t {
    matrix_t m;  // n x n matrix of distances
    matrix_t nn; // n x k matrix of nearest neighbors, node => [nn1, nn2, ...]

    adj_matrix_t(unsigned int n) : m(n, list_t(n, 0)), nn(n, list_t(n)) {}

    list_t &operator[](unsigned int i) { return m.at(i); }
    list_t operator[](unsigned int i) const { return m.at(i); }

    list_t neighbors(unsigned int i, unsigned int k) const {
        return {nn.at(i).begin(), nn.at(i).begin() + k};
    }

    matrix_t::iterator begin() { return m.begin(); }
    matrix_t::iterator end() { return m.end(); }
    matrix_t::const_iterator begin() const { return m.begin(); }
    matrix_t::const_iterator end() const { return m.end(); }
};

struct tsp_t {
    unsigned int path_size;
    unsigned int n;
    std::vector<node_t> nodes;
    std::vector<unsigned int> weights;
    adj_matrix_t adj_matrix;

    tsp_t(std::vector<node_t> nodes, adj_matrix_t adj_matrix)
        : path_size(ceil(nodes.size() / 2.0)), n(nodes.size()), nodes(nodes),
          weights(nodes.size(), 0), adj_matrix(adj_matrix) {
        for (int i = 0; i < n; i++) {
            weights[i] = nodes[i].weight;
        }
    }
};

struct solution_t {
    int cost;
    int runtime_ms;
    int search_iters;
    list_t path;
    std::unordered_set<unsigned int> remaining_nodes;
    const tsp_t *tsp;

    solution_t(const tsp_t &tsp, list_t path, int runtime_ms = 0,
               int search_iters = 0)
        : cost(0), runtime_ms(runtime_ms), search_iters(search_iters),
          path(path), remaining_nodes(), tsp(&tsp) {
        for (unsigned int i = 0; i < tsp.n; i++) {
            remaining_nodes.insert(i);
        }

        for (unsigned int i = 0; i < path.size() - 1; i++) {
            cost += tsp.adj_matrix[path[i]][path[i + 1]] + tsp.weights[path[i]];
            remaining_nodes.erase(path[i]);
        }

        remaining_nodes.erase(path.back());

        cost += tsp.adj_matrix[path.back()][path.front()] +
                tsp.weights[path.back()];
    }

    solution_t(const tsp_t &tsp, unsigned int start)
        : cost(tsp.weights[start]), runtime_ms(0), search_iters(0),
          path({start}), remaining_nodes(), tsp(&tsp) {
        for (unsigned int i = 0; i < tsp.n; i++) {
            if (i == start) {
                continue;
            }

            remaining_nodes.insert(i);
        }
    }

#pragma region Operators

    // Append node to the end of the path ({0, 1, 2} -> {0, 1, 2, node})
    void append(unsigned int node) {
        cost += tsp->adj_matrix[path.back()][node] + tsp->weights[node];
        path.push_back(node);
        remaining_nodes.erase(node);
    }

    // Prepend node to the beginning of the path ({0, 1, 2} -> {node, 0, 1, 2})
    void prepend(unsigned int node) {
        cost += tsp->adj_matrix[node][path.front()] + tsp->weights[node];
        path.insert(path.begin(), node);
        remaining_nodes.erase(node);
    }

    // Insert node after the node at pos ({0, 1, 2} -> 1 -> {0, 1, node, 2})
    void insert(unsigned int node, int pos) {
        cost += insert_delta(node, pos);
        path.insert(path.begin() + pos + 1, node);
        remaining_nodes.erase(node);
    }

    // Delete node at pos ({0, 1, 2} -> 1 -> {0, 2})
    void remove(int pos) {
        cost += remove_delta(pos);
        remaining_nodes.insert(path[pos]);
        path.erase(path.begin() + pos);
    }

    // Replace node at pos with node ({0, 1, 2} -> 1 -> {0, node, 2})
    void replace(unsigned int node, int pos) {
        cost += replace_delta(node, pos);
        remaining_nodes.erase(node);
        remaining_nodes.insert(path[pos]);
        path[pos] = node;
    }

    // Swap nodes at pos1 and pos2 ({0, 1, 2} -> 1, 2 -> {0, 2, 1})
    void swap(int pos1, int pos2) {
        cost += swap_delta(pos1, pos2);
        std::swap(path[pos1], path[pos2]);
    }

    // Reverse path from pos1 to pos2 i.e. swap the edges ({0, 1, 2, 3, 4}
    // -> 1, 3 -> {0, 3, 2, 1, 4})
    void reverse(int pos1, int pos2) {
        if (pos1 > pos2) {
            throw std::logic_error(
                "Trying to reverse from higher to lower pos");
        }

        cost += reverse_delta(pos1, pos2);
        std::reverse(path.begin() + pos1, path.begin() + pos2 + 1);
    }

#pragma endregion Operators

#pragma region Cost functions
    // Cost delta of appending node to the path (see: append)
    int append_delta(unsigned int node) const {
        return tsp->adj_matrix[path.back()][node] + tsp->weights[node];
    }

    // Cost delta of prepending node to the path (see: prepend)
    int prepend_delta(unsigned int node) const {
        return tsp->adj_matrix[node][path.front()] + tsp->weights[node];
    }

    // Cost delta of inserting node at pos (see: insert)
    int insert_delta(unsigned int node, int pos) const {
        unsigned int a = path[pos];
        unsigned int b = path[next(pos)];
        return tsp->weights[node] + tsp->adj_matrix[a][node] +
               tsp->adj_matrix[node][b] - tsp->adj_matrix[a][b];
    }

    int remove_delta(int pos) {
        unsigned a = path[prev(pos)];
        unsigned b = path[pos];
        unsigned c = path[next(pos)];
        return tsp->adj_matrix[a][c] - tsp->adj_matrix[a][b] -
               tsp->adj_matrix[b][c] - tsp->weights[b];
    }

    // Cost delta of replacing node at pos (see: replace)
    int replace_delta(unsigned int node, int pos) const {
        unsigned int old_node = path[pos];
        unsigned int a = path[prev(pos)];
        unsigned int b = path[next(pos)];

        return tsp->adj_matrix[a][node] + tsp->adj_matrix[node][b] +
               tsp->weights[node] - tsp->adj_matrix[a][old_node] -
               tsp->adj_matrix[old_node][b] - tsp->weights[old_node];
    }

    // Cost delta of swapping nodes at pos1 and pos2 (see: swap)
    int swap_delta(int pos1, int pos2) const {
        unsigned node1 = path[pos1];
        unsigned node2 = path[pos2];

        unsigned int a1 = path[prev(pos1)];
        unsigned int b1 = path[next(pos1)];
        unsigned int a2 = path[prev(pos2)];
        unsigned int b2 = path[next(pos2)];

        // next to each other
        if (node2 == b1) {
            return tsp->adj_matrix[a1][node2] + tsp->adj_matrix[node1][b2] -
                   tsp->adj_matrix[a1][node1] - tsp->adj_matrix[node2][b2];
        }

        // loop
        if (node1 == b2) {
            return tsp->adj_matrix[a2][node1] + tsp->adj_matrix[node2][b1] -
                   tsp->adj_matrix[a2][node2] - tsp->adj_matrix[node1][b1];
        }

        return tsp->adj_matrix[a1][node2] + tsp->adj_matrix[node2][b1] +
               tsp->adj_matrix[a2][node1] + tsp->adj_matrix[node1][b2] -
               tsp->adj_matrix[a1][node1] - tsp->adj_matrix[node1][b1] -
               tsp->adj_matrix[a2][node2] - tsp->adj_matrix[node2][b2];
    }

    // Cost delta of reversing the path from pos1 to pos2 (see: reverse)
    int reverse_delta(int pos1, int pos2) const {
        int a = path[prev(pos1)];
        int b = path[pos1];
        int c = path[pos2];
        int d = path[next(pos2)];

        if (a == c) {
            return 0;
        }

        return tsp->adj_matrix[a][c] + tsp->adj_matrix[b][d] -
               tsp->adj_matrix[a][b] - tsp->adj_matrix[c][d];
    }

#pragma endregion Cost functions

#pragma region Helpers

    unsigned int next(unsigned int i) const { return (i + 1) % path.size(); }

    unsigned int prev(unsigned int i) const {
        if (i == 0) {
            return path.size() - 1;
        }

        return (i - 1) % path.size();
    }

    bool is_valid() const {
        if (std::unordered_set(path.begin(), path.end()).size() !=
            path.size()) {
            return false;
        }

        int actual_cost = 0;
        for (unsigned int i = 0; i < path.size() - 1; i++) {
            actual_cost +=
                tsp->weights[path[i]] + tsp->adj_matrix[path[i]][path[i + 1]];
        }
        actual_cost += tsp->weights[path.back()] +
                       tsp->adj_matrix[path.back()][path.front()];
        return actual_cost == cost;
    }

    std::unordered_set<edge_t> to_edges() const {
        std::unordered_set<edge_t> edges;
        edges.reserve(path.size());
        for (unsigned int i = 0; i < path.size() - 1; i++) {
            edges.emplace(edge_t{path[i], path[i + 1]});
        }
        edges.emplace(edge_t{path.back(), path.front()});
        return edges;
    }

#pragma endregion Helpers
};

struct algo_t {
    virtual std::string short_name() const = 0;
    virtual std::string full_name() const = 0;
    virtual std::vector<solution_t>
    run(const tsp_t &tsp,
        std::optional<unsigned int> time_limit_ms = std::nullopt) const = 0;

    virtual ~algo_t() = default;
    algo_t() = default;
    algo_t(const algo_t &other) = default;
    algo_t(algo_t &&other) = default;

    algo_t &operator=(const algo_t &other) { return *this; }
    algo_t &operator=(algo_t &&other) { return *this; }
};

typedef std::pair<unsigned int, int> pos_delta_t;
typedef std::pair<unsigned int, int> node_delta_t;

enum op_type_t { APPEND, PREPEND, INSERT, REPLACE, SWAP, REVERSE };
enum search_t { GREEDY, STEEPEST };

struct operation_t {
    op_type_t type;
    unsigned int arg1, arg2;
    int delta;
};

struct similarity_t {
    int cost;
    unsigned int avg_common_edges, avg_common_nodes;
    double best_common_edges, best_common_nodes;
};

typedef std::function<solution_t(const solution_t &, const solution_t &)>
    evo_op_t;

struct individual_t {
    int cost;
    unsigned int idx;
};

template <> struct std::less<individual_t> {
    constexpr bool operator()(const individual_t &a,
                              const individual_t &b) const {
        return a.cost > b.cost;
    }
};

struct timer_t {
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;

    void start() { start_time = std::chrono::high_resolution_clock::now(); }

    int measure() {
        auto end_time = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::milliseconds>(end_time -
                                                                     start_time)
            .count();
    }
};
