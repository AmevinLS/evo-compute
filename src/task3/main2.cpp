#include <fstream>
#include <iostream>

#include "../common/parse.cpp"
#include "../common/print.cpp"
#include "solve_local_search.cpp"

int main() {
    for (std::string tsp_path :
         {"../../data/TSPA.csv", "../../data/TSPB.csv"}) {
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
                // Random solution
                std::vector<solution_t> res_sols;
                for (const auto &start_sol : start_sols) {
                    res_sols.push_back(
                        searcher.local_search(start_sol, select_method, intra));
                    std::cout << "Finished processing solution "
                              << res_sols.size()
                              << ". Cost: " << res_sols.back().cost << "\n";
                }
                std::string instance_name =
                    tsp_path.substr(tsp_path.find_last_of("/\\") + 1);
                instance_name =
                    instance_name.substr(0, instance_name.find_last_of("."));
                std::ofstream fout("../../results/lolkek" +
                                   std::to_string(select_method) + "_" +
                                   std::to_string(intra) + ".csv");
                fout << res_sols;
                // Best old solution
                // TODO
            }
        }
    }
    return 0;
}