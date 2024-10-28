#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>
#include <set>
#include <utility>
#include <vector>

struct node_t {
    int x;
    int y;
    int weight;
};

typedef std::vector<int> adj_list_t;

struct adj_matrix_t {
    using matrix_t = std::vector<adj_list_t>;
    matrix_t m;

    adj_matrix_t(unsigned int n) : m(n, adj_list_t(n, 0)) {}

    int &operator()(unsigned int i, unsigned int j) { return m.at(i).at(j); }
    int operator()(unsigned int i, unsigned int j) const {
        return m.at(i).at(j);
    }

    adj_list_t &operator[](unsigned int i) { return m.at(i); }
    adj_list_t operator[](unsigned int i) const { return m.at(i); }

    matrix_t::iterator begin() { return m.begin(); }
    matrix_t::iterator end() { return m.end(); }
    matrix_t::const_iterator begin() const { return m.begin(); }
    matrix_t::const_iterator end() const { return m.end(); }
};

struct tsp_t {
    unsigned int n;
    std::vector<int> weights;
    adj_matrix_t adj_matrix;

    tsp_t(std::vector<node_t> nodes, adj_matrix_t adj_matrix)
        : n(nodes.size()), weights(), adj_matrix(adj_matrix) {
        for (auto &node : nodes) {
            weights.push_back(node.weight);
        }
    }
};

struct solution_t {
    int cost;
    std::vector<unsigned int> path;
    std::set<unsigned int> remaining_nodes;
    const tsp_t *tsp;

    solution_t(const tsp_t &tsp, std::vector<unsigned int> path)
        : cost(0), path(path), remaining_nodes(), tsp(&tsp) {
        for (unsigned int i = 0; i < tsp.n; i++) {
            remaining_nodes.insert(i);
        }

        for (unsigned int i = 0; i < path.size() - 1; i++) {
            cost += tsp.adj_matrix(path[i], path[i + 1]) + tsp.weights[path[i]];
            remaining_nodes.erase(path[i]);
        }

        remaining_nodes.erase(path.back());

        cost += tsp.adj_matrix(path.back(), path.front()) +
                tsp.weights[path.back()];
    }

    solution_t(const tsp_t &tsp, unsigned int start)
        : cost(tsp.weights[start]), path({start}), remaining_nodes(),
          tsp(&tsp) {
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
        cost += tsp->adj_matrix(path.back(), node) + tsp->weights[node];
        path.push_back(node);
        remaining_nodes.erase(node);
    }

    // Prepend node to the beginning of the path ({0, 1, 2} -> {node, 0, 1, 2})
    void prepend(unsigned int node) {
        cost += tsp->adj_matrix(node, path.front()) + tsp->weights[node];
        path.insert(path.begin(), node);
        remaining_nodes.erase(node);
    }

    // Insert node after the node at pos ({0, 1, 2} -> 1 -> {0, 1, node, 2})
    void insert(unsigned int node, int pos) {
        cost += insert_delta(node, pos);
        path.insert(path.begin() + pos + 1, node);
        remaining_nodes.erase(node);
    }

    // Replace node at pos with node ({0, 1, 2} -> 1 -> {0, node, 2})
    void replace(unsigned int node, int pos) {
        cost += replace_delta(node, pos);
        unsigned int old_node = path[pos];
        path[pos] = node;
        remaining_nodes.erase(node);
        remaining_nodes.insert(old_node);
    }

    // Swap nodes at pos1 and pos2 ({0, 1, 2} -> 1, 2 -> {0, 2, 1})
    void swap(int pos1, int pos2) {
        cost += swap_delta(pos1, pos2);
        std::swap(path[pos1], path[pos2]);
    }

    // Reverse path from pos1 to pos2 i.e. swap the edges ({0, 1, 2, 3, 4}
    // -> 1, 3 -> {0, 3, 2, 1, 4})
    void reverse(int pos1, int pos2) {
        cost += reverse_delta(pos1, pos2);
        std::reverse(path.begin() + pos1, path.begin() + pos2 + 1);
    }

    // Add the cost of the edge from the last node to the first node (for
    // the methods that don't include that cost by default)
    void commit() { cost += tsp->adj_matrix(path.back(), path.front()); }

#pragma endregion Operators

#pragma region Cost functions
    // Cost delta of appending node to the path (see: append)
    int append_delta(unsigned int node) const {
        return tsp->adj_matrix(path.back(), node) + tsp->weights[node];
    }

    // Cost delta of prepending node to the path (see: prepend)
    int prepend_delta(unsigned int node) const {
        return tsp->adj_matrix(node, path.front()) + tsp->weights[node];
    }

    // Cost delta of inserting node at pos (see: insert)
    int insert_delta(unsigned int node, int pos) const {
        unsigned int a = path[pos];
        unsigned int b = path[next(pos)];
        return tsp->weights[node] + tsp->adj_matrix(a, node) +
               tsp->adj_matrix(node, b) - tsp->adj_matrix(a, b);
    }

    // Cost delta of replacing node at pos (see: replace)
    int replace_delta(unsigned int node, int pos) const {
        unsigned int old_node = path[pos];
        unsigned int a = path[prev(pos)];
        unsigned int b = path[next(pos)];

        return tsp->adj_matrix(a, node) + tsp->adj_matrix(node, b) +
               tsp->weights[node] - tsp->adj_matrix(a, old_node) -
               tsp->adj_matrix(old_node, b) - tsp->weights[old_node];
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
            return tsp->adj_matrix(a1, node2) + tsp->adj_matrix(node1, b2) -
                   tsp->adj_matrix(a1, node1) - tsp->adj_matrix(node2, b2);
        }

        // loop
        if (node1 == b2) {
            return tsp->adj_matrix(a2, node1) + tsp->adj_matrix(node2, b1) -
                   tsp->adj_matrix(a2, node2) - tsp->adj_matrix(node1, b1);
        }

        return tsp->adj_matrix(a1, node2) + tsp->adj_matrix(node2, b1) +
               tsp->adj_matrix(a2, node1) + tsp->adj_matrix(node1, b2) -
               tsp->adj_matrix(a1, node1) - tsp->adj_matrix(node1, b1) -
               tsp->adj_matrix(a2, node2) - tsp->adj_matrix(node2, b2);
    }

    // Cost delta of reversing the path from pos1 to pos2 (see: reverse)
    int reverse_delta(int pos1, int pos2) const {
        int a = path[prev(pos1)];
        int b = path[pos1];
        int c = path[pos2];
        int d = path[next(pos2)];

        if (a == c || b == d) {
            return 0;
        }

        return tsp->adj_matrix(a, c) + tsp->adj_matrix(b, d) -
               tsp->adj_matrix(a, b) - tsp->adj_matrix(c, d);
    }

#pragma endregion Cost functions

#pragma region Helpers

    int next(int i) const { return (i + 1) % path.size(); }

    int prev(int i) const { return (i - 1 + path.size()) % path.size(); }

    bool is_valid() const {
        return std::set(path.begin(), path.end()).size() == path.size() &&
               path.size() == ceil(tsp->n / 2.0);
    }

#pragma endregion Helpers
};

typedef std::pair<unsigned int, int> pos_delta_t;

typedef std::pair<unsigned int, int> node_delta_t;

enum expand_t { APPEND, PREPEND, INSERT };
enum intra_path_t { SWAP, REVERSE };
enum inter_path_t { REPLACE };

struct operation_t {
    std::function<void(solution_t &)> func;
    int delta;
};
