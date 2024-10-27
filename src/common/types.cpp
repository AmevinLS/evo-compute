#pragma once

#include <algorithm>
#include <set>
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

    void append(unsigned int node) {
        cost += tsp->adj_matrix(path.back(), node) + tsp->weights[node];
        path.push_back(node);
        remaining_nodes.erase(node);
    }

    void prepend(unsigned int node) {
        cost += tsp->adj_matrix(node, path.front()) + tsp->weights[node];
        path.insert(path.begin(), node);
        remaining_nodes.erase(node);
    }

    void insert(unsigned int node, int pos) {
        cost += get_cost_diff(node, pos);
        path.insert(path.begin() + pos + 1, node);
        remaining_nodes.erase(node);
    }

    int swap_nodes_cost_diff(unsigned idx1, unsigned idx2) const {
        if (idx1 > idx2) {
            std::swap(idx1, idx2);
        }

        unsigned node1 = path[idx1];
        unsigned node2 = path[idx2];
        int cost_diff = 0;
        if (idx1 + 1 == idx2) {
            cost_diff -= tsp->adj_matrix(path[shift_idx(idx1, -1)], node1) +
                         tsp->adj_matrix(node2, path[shift_idx(idx2, 1)]);
            cost_diff += tsp->adj_matrix(path[shift_idx(idx1, -1)], node2) +
                         tsp->adj_matrix(node1, path[shift_idx(idx2, 1)]);
        } else {
            cost_diff -= tsp->adj_matrix(path[shift_idx(idx1, -1)], node1) +
                         tsp->adj_matrix(node1, path[shift_idx(idx1, 1)]) +
                         tsp->adj_matrix(path[shift_idx(idx2, -1)], node2) +
                         tsp->adj_matrix(node2, path[shift_idx(idx2, 1)]);
            cost_diff += tsp->adj_matrix(path[shift_idx(idx1, -1)], node2) +
                         tsp->adj_matrix(node2, path[shift_idx(idx1, 1)]) +
                         tsp->adj_matrix(path[shift_idx(idx2, -1)], node1) +
                         tsp->adj_matrix(node1, path[shift_idx(idx2, 1)]);
        }
        return cost_diff;
    }

    void swap_nodes(unsigned idx1, unsigned idx2) {
        cost += swap_nodes_cost_diff(idx1, idx2);
        std::swap(path[idx1], path[idx2]);
    }

    int swap_edges_cost_diff(unsigned pos1, unsigned pos2) const {
        int cost_diff = 0;
        cost_diff -= tsp->adj_matrix(path[pos1], shift_idx(pos1, 1)) +
                     tsp->adj_matrix(path[pos2], shift_idx(pos2, 1));
        cost_diff += tsp->adj_matrix(path[pos1], path[pos2]) +
                     tsp->adj_matrix(shift_idx(pos1, 1), shift_idx(pos2, 1));
        return cost_diff;
    }

    void swap_edges(unsigned pos1, unsigned pos2) {
        if (pos1 > pos2) {
            std::swap(pos1, pos2);
        }
        cost += swap_edges_cost_diff(pos1, pos2);
        std::reverse(path.begin() + pos1 + 1, path.begin() + pos2 + 1);
    }

    int replace_node_cost_diff(unsigned idx, unsigned new_node) const {
        unsigned old_node = path[idx];
        int cost_diff = 0;
        cost_diff -= tsp->adj_matrix(path[shift_idx(idx, -1)], old_node) +
                     tsp->adj_matrix(old_node, path[shift_idx(idx, 1)]);
        cost_diff += tsp->adj_matrix(path[shift_idx(idx, -1)], new_node) +
                     tsp->adj_matrix(new_node, path[shift_idx(idx, 1)]);
        return cost_diff;
    }

    void replace_node(unsigned idx, unsigned new_node) {
        // TODO: check if new_node already in path?
        unsigned old_node = path[idx];
        cost += replace_node_cost_diff(idx, new_node);
        path[idx] = new_node;
    }

    unsigned shift_idx(unsigned idx, int steps) const {
        if (steps == 0)
            return idx;

        unsigned pre_remainder;
        if (steps < 0)
            pre_remainder = idx;
        else
            pre_remainder = path.size() - idx;

        unsigned post_remainder = (steps - pre_remainder) % path.size();
        unsigned result_idx;
        if (steps < 0)
            result_idx = path.size() - post_remainder;
        else
            result_idx = post_remainder;

        return result_idx;
    }

    void commit() { cost += tsp->adj_matrix(path.back(), path.front()); }

    int get_cost_diff(unsigned int node, int pos) const {
        unsigned int a = path[pos];
        unsigned int b = path[(pos + 1) % path.size()];
        return tsp->weights[node] + tsp->adj_matrix(a, node) +
               tsp->adj_matrix(node, b) - tsp->adj_matrix(a, b);
    }
};
