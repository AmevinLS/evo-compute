#include "common/parse.cpp"
#include "common/print.cpp"
#include "common/types.cpp"
#include "solve.cpp"
#include <fstream>

int main() {
    std::string fname = "../data/TSPA.csv";
    std::ifstream fin(fname);

    if (!fin.is_open()) {
        return 1;
    }

    tsp_t tsp = parse(fin);
    std::string instance_name = fname.substr(fname.find_last_of("/\\") + 1);
    instance_name = instance_name.substr(0, instance_name.find_last_of("."));

    for (auto &heur : {RANDOM}) {
        std::string heur_str = heuristic_t_str[heur];
        std::vector solutions = solve(tsp, heur);
        std::ofstream out("../results/" + instance_name + "_" + heur_str +
                          ".csv");

        out << solutions;
    }

    return 0;
}
