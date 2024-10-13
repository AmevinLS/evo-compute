#include <vector>

struct node_t {
  int x;
  int y;
  int weight;
};

struct adj_matrix_t {
  std::vector<std::vector<int>> m;

  adj_matrix_t(unsigned int n) : m(n, std::vector<int>(n, 0)) {}

  struct row_t {
    std::vector<int> &r;
    row_t(std::vector<int> &r) : r(r) {}
    int &operator[](unsigned int j) { return r.at(j); }
  };

  int &operator()(unsigned int i, unsigned int j) { return m.at(i).at(j); }

  row_t operator[](unsigned int i) { return row_t(m.at(i)); }
};

struct tsp_t {
  unsigned int n;
  std::vector<node_t> nodes;
  adj_matrix_t adj_matrix;
};
