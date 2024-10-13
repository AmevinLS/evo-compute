#include <fstream>
#include <iostream>
#include <optional>
#include <ostream>

#include "parse.cpp"
#include "print.cpp"

#define ERROR "\033[0;31m[ERROR]\033[0m"

std::optional<std::ifstream> open_file(const std::string &fname) {
  std::ifstream in(fname);
  if (!in.is_open()) {
    std::cerr << ERROR << " failed to open file: " << fname << std::endl;
    std::cerr << "Check if the file exists and you have permission to read it."
              << std::endl;
    return {};
  }
  return in;
}

int parse_main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << ERROR << " usage: " << argv[0] << " parse <file> [options]"
              << std::endl;
    return 1;
  }

  std::string fname = argv[2];
  bool verbose = false;

  int i = 2;
  while (++i < argc) {
    if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
      verbose = true;
    } else {
      std::cerr << ERROR << " unknown option: " << argv[i] << std::endl;
      return 1;
    }
  }

  auto in = open_file(fname);
  if (!in.has_value()) {
    return 1;
  }

  tsp_t tsp = parse(in.value());

  std::cout << tsp.n << " nodes" << std::endl;

  if (verbose) {
    std::cout << "Nodes:" << std::endl;
    std::cout << tsp.nodes << std::endl;
    std::cout << std::endl;
    std::cout << "Adjacency matrix:" << std::endl;
    std::cout << tsp.adj_matrix << std::endl;
  }

  return 0;
}

int main(int argc, char **argv) {
  if (argc == 1 || (argc == 2 && strcmp(argv[1], "--help") == 0)) {
    std::cout << "usage " << argv[0] << " <command> [args]" << std::endl
              << std::endl;
    std::cout << "commands:" << std::endl;
    std::cout << "\t--help\t\tShow this help message" << std::endl;
    std::cout << "\t--version\t\tShow version information" << std::endl;
    std::cout << "\tparse\t\tParse the data file and print it to STDOUT"
              << std::endl;
    std::cout << "\tsolve\t\tSolve the TSP problem" << std::endl;
    return 0;
  }

  if (argc == 2 && strcmp(argv[1], "--version") == 0) {
    std::cout << "TSP Solver v0.1" << std::endl;
    return 0;
  }

  if (strcmp(argv[1], "parse") == 0) {
    return parse_main(argc, argv);
  }

  std::cerr << ERROR << " unknown command: " << argv[1] << std::endl;
  return 1;
}
