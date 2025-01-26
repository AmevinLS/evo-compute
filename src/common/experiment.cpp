#pragma once

#include "parse.cpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

int run_experiment(const std::string &fname, const std::string &output_dir,
                   const std::vector<std::shared_ptr<algo_t>> algorithms,
                   unsigned int time_limit_ms) {
    auto tsp = parse(fname);

    if (!tsp.has_value()) {
        return 1;
    }

    std::string out_path = output_dir + instance_name(fname);

    for (auto algo : algorithms) {
        std::string path = out_path + "_" + algo->short_name() + ".csv";
        std::cout << "Running " << algo->full_name()
                  << " heuristic and saving to " << path << "..." << std::endl;

        std::vector<solution_t> solutions =
            algo->run(tsp.value(), time_limit_ms);
        std::ofstream out(path);

        out << solutions;
    }

    return 0;
}

int run_experiment(const std::string &fname,
                   const std::vector<std::shared_ptr<algo_t>> algorithms,
                   unsigned int time_limit_ms) {
    auto tsp = parse(fname);
    if (!tsp.has_value()) {
        return 1;
    }

    for (auto algo : algorithms) {
        std::vector<solution_t> solutions =
            algo->run(tsp.value(), time_limit_ms);
        std::cout << algo->full_name() << ":\n" << solutions;
    }

    return 0;
}

int find_similarities(
    const std::string &fname, const std::string &output_dir,
    const std::function<std::vector<similarity_t>(const tsp_t &)>
        gen_similarities) {
    auto tsp = parse(fname);

    if (!tsp.has_value()) {
        return 1;
    }

    std::ofstream fout(output_dir + instance_name(fname) + "_sim.csv");

    std::vector<similarity_t> sims = gen_similarities(tsp.value());
    fout << sims;

    return 0;
}
