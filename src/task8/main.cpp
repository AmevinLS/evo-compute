#include <fstream>
#include <iostream>
#include <ostream>
#include <vector>

#include "../common/parse.cpp"
#include "../common/types.cpp"
#include "../task3/solve_local_search.cpp"
#include "similarities.cpp"

void save_similarities(const std::vector<solution_t> &sols,
                       unsigned best_sol_idx, std::string filepath) {
    std::ofstream fout(filepath);
    fout << "cost,num_common_edges,num_common_nodes,avg_common_edges,avg_"
            "common_nodes\n";
    for (unsigned i = 0; i < sols.size(); i++) {
        if (i % 100 == 0)
            std::cout << "Calculating similarities for solution " << i << "\n";
        // if (i == best_sol_idx)
        //     continue;
        unsigned common_edges_with_best =
            common_edges_similarity(sols[i], sols[best_sol_idx]);
        unsigned common_nodes_with_best =
            common_nodes_similarity(sols[i], sols[best_sol_idx]);

        double common_edges_sum = 0, common_nodes_sum = 0;
        for (unsigned j = 0; j < sols.size(); j++) {
            if (j == i)
                continue;
            common_edges_sum += common_edges_similarity(sols[i], sols[j]);
            common_nodes_sum += common_nodes_similarity(sols[i], sols[j]);
        }
        double denominator = static_cast<double>(sols.size() - 1);
        fout << sols[i].cost << "," << common_edges_with_best << ","
             << common_nodes_with_best << "," << common_edges_sum / denominator
             << "," << common_nodes_sum / denominator << "\n";
    }
}

void process_instance(std::string instance) {
    std::ifstream fin("../../data/" + instance + ".csv");
    tsp_t tsp = parse(fin);

    const unsigned NUM_SOLUTIONS = 1000;
    const unsigned PATH_SIZE = 100;

    std::vector<solution_t> sols;
    unsigned best_sol_idx = 0;
    for (unsigned i = 0; i < NUM_SOLUTIONS; i++) {
        if (i % 100 == 0)
            std::cout << "Generating solution " << i << std::endl;
        solution_t sol = gen_random_solution(tsp, PATH_SIZE);
        sol = solve_local_search(sol, solution_t::REVERSE, GREEDY);
        sols.push_back(sol);

        if (sol.cost < sols[best_sol_idx].cost)
            best_sol_idx = i;
    }

    std::cout << "\nBest solution has cost " << sols[best_sol_idx].cost
              << "\n\n";

    save_similarities(sols, best_sol_idx,
                      "../../results" + instance + "_similarities.csv");
}

int main() {
    std::cout << "Processing instnace TSPA\n\n";
    process_instance("TSPA");
    std::cout << "=========================================\n";
    std::cout << "Processing instance TSPB\n\n";
    process_instance("TSPB");
}
