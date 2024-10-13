#include <iostream>
#include <ostream>
#include <vector>

#include "types.cpp"

std::ostream &operator<<(std::ostream &os, adj_list_t list) {
  int i = 1;
  for (const int &x : list) {
    os << "\t" << i++ << ": " << x << std::endl;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, adj_matrix_t matrix) {
  int i = 1;
  for (const adj_list_t &row : matrix) {
    os << i++ << ": " << std::endl << row << std::endl;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, node_t node) {
  os << "(" << node.x << ", " << node.y << ") weight: " << node.weight;
  return os;
}

std::ostream &operator<<(std::ostream &os, std::vector<node_t> nodes) {
  int i = 1;
  for (const node_t &node : nodes) {
    os << i++ << node << std::endl;
  }
  return os;
}

std::ostream &operator<<(std::ostream &os, solution_t solution) {
  os << "Cost: " << solution.cost << std::endl;
  os << "Path: " << std::endl;

  for (const unsigned int &node : solution.path) {
    os << node + 1 << std::endl;
  }

  return os;
}
