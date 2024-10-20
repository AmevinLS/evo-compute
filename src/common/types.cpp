#pragma once

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
  int operator()(unsigned int i, unsigned int j) const { return m.at(i).at(j); }

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

  solution_t() : cost(0), path() {}

  solution_t(int cost, std::vector<unsigned int> path)
      : cost(cost), path(path) {}
};
