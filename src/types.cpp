#include <vector>

struct node_t {
  int x;
  int y;
  int weight;
};

typedef std::vector<int> adj_list_t;

struct adj_matrix_t {
  std::vector<std::vector<int>> m;

  adj_matrix_t(unsigned int n) : m(n, std::vector<int>(n, 0)) {}

  int &operator()(unsigned int i, unsigned int j) { return m.at(i).at(j); }

  adj_list_t operator[](unsigned int i) { return adj_list_t(m.at(i)); }
};

struct tsp_t {
  unsigned int n;
  std::vector<node_t> nodes;
  adj_matrix_t adj_matrix;

  tsp_t(std::vector<node_t> nodes, adj_matrix_t adj_matrix)
      : n(nodes.size()), nodes(nodes), adj_matrix(adj_matrix) {}
};
