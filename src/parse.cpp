#include <cmath>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "types.cpp"

int l2(const node_t a, const node_t b) {
  return int(round(sqrt(pow(b.x - a.x, 2) + pow(b.y - a.y, 2))));
}

adj_matrix_t matrixof(const std::vector<node_t> nodes) {
  adj_matrix_t matrix(nodes.size());

  for (unsigned int i = 0; i < nodes.size(); i++) {
    for (unsigned int j = 0; j < nodes.size(); j++) {
      if (i == j) {
        continue;
      }

      matrix[i][j] = l2(nodes[i], nodes[j]);
    }
  }

  return matrix;
}

std::vector<node_t> read(std::ifstream &in) {
  std::vector<node_t> nodes;
  std::string line;

  while (std::getline(in, line)) {
    std::stringstream ss(line);
    std::string value;
    node_t node;

    std::getline(ss, value, ';');
    node.x = std::stoi(value);
    std::getline(ss, value, ';');
    node.y = std::stoi(value);
    std::getline(ss, value, ';');
    node.weight = std::stoi(value);

    nodes.push_back(node);
  }

  return nodes;
}

tsp_t parse(std::ifstream &in) {
  std::vector<node_t> nodes = read(in);
  adj_matrix_t matrix = matrixof(nodes);
  return tsp_t(nodes, matrix);
}
