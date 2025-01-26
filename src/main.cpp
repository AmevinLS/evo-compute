#include "common/experiment.cpp"
#include "common/types.cpp"
#include "solvers/solvers.cpp"
#include "solvers/task6.cpp"

#include <memory>

int main() {
    std::string instance_name = "TSPA";
    std::string fname = "data/" + instance_name + ".csv";

    auto tsp = parse(fname);

    if (!tsp.has_value()) {
        return 1;
    }

    unsigned int time_limit_ms = calc_time_limit_ms(tsp.value());
    std::vector<std::shared_ptr<algo_t>> algos = {
        std::make_shared<random_algo>(),
        std::make_shared<greedy_cycle_regret_weighted_algo>(),
        std::make_shared<large_neighborhood_search_ls_algo>(),
        std::make_shared<hybrid_evo_repair_ls_algo>(),
    };

    return run_experiment(fname, algos, time_limit_ms);
}
