#include <chrono>
#include <fstream>
#include <iostream>
#include <vector>

#include "../common/parse.cpp"
#include "../common/print.cpp"
#include "solve_local_search.cpp"

std::ostream &operator<<(std::ostream &os, const std::vector<unsigned> &path) {
    for (unsigned i = 0; i < path.size() - 1; i++) {
        os << path[i] << " ";
    }
    os << path.back();
    return os;
}

int main() {
    for (std::string tsp_path :
         {"../../data/TSPA.csv", "../../data/TSPB.csv"}) {
        std::string instance_name =
            tsp_path.substr(tsp_path.find_last_of("/\\") + 1);
        instance_name =
            instance_name.substr(0, instance_name.find_last_of("."));

        std::ifstream fin(tsp_path);
        tsp_t tsp = parse(fin);
        LocalSearcher searcher(tsp);

        std::vector<solution_t> start_sols;
        for (unsigned i = 0; i < 200; i++) {
            start_sols.push_back(random_solution(tsp, 100));
        }
        for (auto select_method : {LOCAL_GREEDY, LOCAL_STEEPEST}) {
            for (auto intra : {INTRA_NODE_SWAP, INTRA_EDGE_SWAP}) {
                std::cout << tsp_path << " | " << select_method << " | "
                          << intra << "\n";
                std::ofstream fout("../../results/localsearch_" +
                                   instance_name + "_" +
                                   std::to_string(select_method) + "_" +
                                   std::to_string(intra) + ".csv");
                fout << "idx,time,cost,path\n";
                for (const auto &start_sol : start_sols) {
                    auto start = std::chrono::high_resolution_clock::now();
                    solution_t best_sol =
                        searcher.local_search(start_sol, select_method, intra);
                    auto end = std::chrono::high_resolution_clock::now();
                    fout << best_sol.path[0] << "," << (end - start).count()
                         << "," << best_sol.cost << "," << best_sol.path
                         << "\n";
                    std::cout << "Finished processing solution "
                              << best_sol.path[0] << ". Cost: " << best_sol.cost
                              << "\n";
                }
                // Best old solution
                // TODO
            }
        }
    }
    return 0;
}