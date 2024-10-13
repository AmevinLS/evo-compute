#include <iostream>

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
