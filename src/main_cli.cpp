#include "common/experiment.cpp"
#include "common/parse.cpp"
#include "common/print.cpp"
#include "common/types.cpp"
#include "solvers/solvers.cpp"

#include <cstring>
#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

int parse_main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << ERROR << " usage: " << argv[0] << " parse <file> [options]"
                  << std::endl;
        return 1;
    }

    if (strcmp(argv[2], "--help") == 0) {
        std::cout << "usage: " << argv[0] << " parse <file> [options]"
                  << std::endl;
        std::cout << "options:" << std::endl;
        std::cout << "\t-v, --verbose\tPrint additional information"
                  << std::endl;
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

    auto tsp = parse(fname);

    if (!tsp.has_value()) {
        return 1;
    }

    std::cout << tsp->n << " nodes" << std::endl;

    if (verbose) {
        std::cout << tsp.value() << std::endl;
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
        std::string algos = "";
        for (int i = 0; i < DEFAULT_ALGOS.size(); i++) {
            algos += "\"" + DEFAULT_ALGOS[i]->short_name() + "\"";
            if (i < DEFAULT_ALGOS.size()) {
                algos += ", ";
            }
        }

        std::cout << "usage: " << argv[0] << " solve <file> [options]"
                  << std::endl;
        std::cout << "options:" << std::endl;
        std::cout << "\t--algo string\tAlgorithms to use (" + algos +
                         ") (default \"random\")"
                  << std::endl;
        return 0;
    }

    std::string fname = argv[2];
    int chosen_algo = 0;

    int i = 2;
    while (++i < argc) {
        if (strcmp(argv[i], "--algo") == 0) {
            if (i + 1 >= argc) {
                std::cerr << ERROR << " missing argument for --algo"
                          << std::endl;
                return 1;
            }

            bool algo_flag = false;
            for (int j = 0; j < DEFAULT_ALGOS.size(); j++) {
                if (argv[i + 1] == DEFAULT_ALGOS[j]->short_name()) {
                    chosen_algo = j;
                    algo_flag = true;
                    break;
                }
            }

            if (!algo_flag) {
                std::cerr << ERROR << " unknown algo: " << argv[i + 1]
                          << std::endl;
                return 1;
            }

            i++;
            continue;
        }

        std::cerr << ERROR << " unknown algo: " << argv[i] << std::endl;
        return 1;
    }

    auto tsp = parse(fname);
    if (!tsp.has_value()) {
        return 1;
    }

    std::vector<solution_t> solutions =
        DEFAULT_ALGOS[chosen_algo]->run(tsp.value());
    std::cout << solutions;

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
        std::cout
            << "\t-o, --output string\tOutput directory (if not specified, "
               "prints results to STDOUT)"
            << std::endl;
        return 0;
    }

    std::string fname = argv[2];
    std::optional<std::string> output_dir = std::nullopt;

    int i = 2;
    while (++i < argc) {
        if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 >= argc) {
                std::cerr << ERROR << " missing argument for --output"
                          << std::endl;
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

    std::vector<std::string> fnames = {};

    if (std::filesystem::is_regular_file(fname)) {
        fnames.push_back(fname);
    } else {
        for (const auto &entry : std::filesystem::directory_iterator(fname)) {
            if (entry.is_regular_file()) {
                fnames.push_back(entry.path().string());
            }
        }
    }

    for (const std::string &fname : fnames) {
        auto tsp = parse(fname);

        if (!tsp.has_value()) {
            std::cerr << ERROR << " failed to parse " << fname << std::endl;
            return 1;
        }

        unsigned int time_limit_ms = calc_time_limit_ms(tsp.value());
        int result = output_dir.has_value()
                         ? run_experiment(fname, output_dir.value(),
                                          DEFAULT_ALGOS, time_limit_ms)
                         : run_experiment(fname, DEFAULT_ALGOS, time_limit_ms);

        if (result != 0) {
            return result;
        }
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
        std::cout << "\texperiment\t\tRun all methods on the instance"
                  << std::endl;
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
