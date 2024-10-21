#include <algorithm>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <optional>
#include <ostream>
#include <string>

#include "common/parse.cpp"
#include "common/print.cpp"
#include "solve.cpp"

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

int experiment(const std::string &fname, const std::string &output_dir) {
  auto in = open_file(fname);

  if (!in.has_value()) {
    return 1;
  }

  tsp_t tsp = parse(in.value());
  std::string instance_name = fname.substr(fname.find_last_of("/\\") + 1);
  instance_name = instance_name.substr(0, instance_name.find_last_of("."));
  instance_name = output_dir + instance_name;

  for (auto &[key, value] : heuristic_t_str) {
    std::cout << "Running " << value << " heuristic" << std::endl;

    std::vector solutions = solve(tsp, key);
    std::string path = instance_name + "_" + value + ".csv";
    std::ofstream out(path);

    out << solutions;

    std::cout << "Results saved to " << path << std::endl;
  }

  return 0;
}

int parse_main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << ERROR << " usage: " << argv[0] << " parse <file> [options]"
              << std::endl;
    return 1;
  }

  if (strcmp(argv[2], "--help") == 0) {
    std::cout << "usage: " << argv[0] << " parse <file> [options]" << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "\t-v, --verbose\tPrint additional information" << std::endl;
    return 0;
  }

  std::string fname = argv[2];
  bool verbose = false;

  int i = 2;
  while (++i < argc) {
    if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
      verbose = true;
      continue;
    }

    std::cerr << ERROR << " unknown option: " << argv[i] << std::endl;
    return 1;
  }

  auto in = open_file(fname);
  if (!in.has_value()) {
    return 1;
  }

  tsp_t tsp = parse(in.value());

  std::cout << tsp.n << " nodes" << std::endl;

  if (verbose) {
    std::cout << "Nodes:" << std::endl;
    std::cout << tsp.weights << std::endl;
    std::cout << std::endl;
    std::cout << "Adjacency matrix:" << std::endl;
    std::cout << tsp.adj_matrix << std::endl;
  }

  return 0;
}

int solve_main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << ERROR << " usage: " << argv[0] << " solve <file> [options]"
              << std::endl;
    return 1;
  }

  if (strcmp(argv[2], "--help") == 0) {
    std::string heuristics = "";
    int i = 0;
    for (auto &[key, value] : heuristic_t_str) {
      heuristics += "\"" + value + "\"";
      if (++i < heuristic_t_str.size()) {
        heuristics += ", ";
      }
    }

    std::cout << "usage: " << argv[0] << " solve <file> [options]" << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "\t--heuristic string\tHeuristic to use (" + heuristics +
                     ") (default \"random\")"
              << std::endl;
    return 0;
  }

  std::string fname = argv[2];
  heuristic_t heuristic = RANDOM;

  int i = 2;
  while (++i < argc) {
    if (strcmp(argv[i], "--heuristic") == 0) {
      if (i + 1 >= argc) {
        std::cerr << ERROR << " missing argument for --heuristic" << std::endl;
        return 1;
      }

      bool heuristic_flag = false;
      for (auto &[key, value] : heuristic_t_str) {
        if (argv[i + 1] == value) {
          heuristic = key;
          heuristic_flag = true;
          break;
        }
      }

      if (!heuristic_flag) {
        std::cerr << ERROR << " unknown heuristic: " << argv[i + 1]
                  << std::endl;
        return 1;
      }

      i++;
      continue;
    }

    std::cerr << ERROR << " unknown option: " << argv[i] << std::endl;
    return 1;
  }

  auto in = open_file(fname);
  if (!in.has_value()) {
    return 1;
  }

  tsp_t tsp = parse(in.value());
  std::vector solutions = solve(tsp, heuristic);

  std::cout << "Solutions:" << std::endl;
  std::cout << solutions << std::endl;
  std::cout << std::endl
            << std::endl
            << "Found " << solutions.size() << " solutions" << std::endl;
  auto best = std::min_element(
      solutions.begin(), solutions.end(),
      [](solution_t a, solution_t b) { return a.cost < b.cost; });

  if (best == solutions.end()) {
    std::cerr << ERROR << " failed to find best solution" << std::endl;
    return 1;
  }

  std::cout << "Best solution:" << std::endl;
  std::cout << *best << std::endl;

  return 0;
}

int experiment_main(int argc, char **argv) {
  if (argc < 3) {
    std::cerr << ERROR << " usage: " << argv[0]
              << " experiment <file> [options]" << std::endl;
    return 1;
  }

  if (strcmp(argv[2], "--help") == 0) {
    std::cout << "usage: " << argv[0] << " experiment <file>" << std::endl;
    std::cout << "options:" << std::endl;
    std::cout << "\t-o, --output string\tOutput directory (default ./results/)"
              << std::endl;
    return 0;
  }

  std::string fname = argv[2];
  std::string output_dir = "./results/";

  int i = 2;
  while (++i < argc) {
    if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
      if (i + 1 >= argc) {
        std::cerr << ERROR << " missing argument for --output" << std::endl;
        return 1;
      }

      std::string output_dir = argv[i + 1];
      if (output_dir.back() != '/') {
        output_dir += '/';
      }

      i++;
      continue;
    }

    std::cerr << ERROR << " unknown option: " << argv[i] << std::endl;
    return 1;
  }

  if (std::filesystem::is_directory(fname)) {
    for (const auto &entry : std::filesystem::directory_iterator(fname)) {
      if (!entry.is_regular_file()) {
        continue;
      }

      int result = experiment(entry.path().string(), output_dir);
      if (result != 0) {
        return result;
      }
    }

    return 0;
  }

  return experiment(fname, output_dir);
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
    std::cout << "\texperiment\t\tRun all methods on the instance" << std::endl;
    return 0;
  }

  if (argc == 2 && strcmp(argv[1], "--version") == 0) {
    std::cout << "TSP Solver v0.1" << std::endl;
    return 0;
  }

  if (strcmp(argv[1], "parse") == 0) {
    return parse_main(argc, argv);
  }

  if (strcmp(argv[1], "solve") == 0) {
    return solve_main(argc, argv);
  }

  if (strcmp(argv[1], "experiment") == 0) {
    return experiment_main(argc, argv);
  }

  std::cerr << ERROR << " unknown command: " << argv[1] << std::endl;
  return 1;
}
