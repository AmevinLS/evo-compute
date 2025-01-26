#include "../common/parse.cpp"
#include "../common/types.cpp"

#include "algorithms.cpp"
#include "own_method.cpp"
#include <iostream>
#include <limits>
#include <memory>
#include <sstream>

struct Stats {
    const double avg_duration_ms;
    const unsigned best_cost;
    const double avg_cost;
    const unsigned worst_cost;
};

class ExperimentRunner {
  private:
    Stats RunAlgoStats(std::shared_ptr<Algorithm> algo,
                       unsigned time_limit_ms) const {
        timer_t timer;
        // std::vector<solution_t> solutions_msls;
        unsigned best_cost = std::numeric_limits<unsigned>().max(),
                 worst_cost = 0, total_cost = 0, total_duration = 0;
        for (unsigned i = 0; i < num_runs_; i++) {
            std::cout << algo->GetName() << " " << i << std::endl;
            timer.start();
            solution_t sol = algo->Run(tsp_, path_size_, time_limit_ms);
            sol.runtime_ms = timer.measure();

            if (sol.cost < best_cost)
                best_cost = sol.cost;
            if (sol.cost > worst_cost)
                worst_cost = sol.cost;
            total_cost += sol.cost;
            total_duration += sol.runtime_ms;

            // solutions_msls.push_back(sol);
        }
        return Stats{static_cast<double>(total_duration) / num_runs_, best_cost,
                     static_cast<double>(total_cost) / num_runs_, worst_cost};
    }

  public:
    ExperimentRunner(const tsp_t &tsp, unsigned num_runs, unsigned path_size)
        : tsp_(tsp), num_runs_(num_runs), path_size_(path_size) {}

    void Run(const std::vector<std::shared_ptr<Algorithm>> &algos) {
        auto msls = std::make_shared<MultipleStart>();
        Stats msls_stats = RunAlgoStats(msls, 1000000);
        std::cout << FormatStats(msls->GetName(), msls_stats);
        unsigned time_limit_ms = msls_stats.avg_duration_ms;
        for (auto algo : algos) {
            Stats stats = RunAlgoStats(algo, time_limit_ms);
            std::cout << FormatStats(algo->GetName(), stats);
        }
    }

    std::string FormatStats(std::string algo_name, const Stats &stats) {
        std::stringstream ss;
        ss << algo_name << ": " << stats.best_cost << " / " << stats.avg_cost
           << " / " << stats.worst_cost << std::endl;
        return ss.str();
    }

    const tsp_t &tsp_;
    const unsigned num_runs_;
    const unsigned path_size_;
};

int main() {
    std::ifstream fin("../../data/TSPA.csv");
    tsp_t tsp = parse(fin);

    const unsigned NUM_RUNS = 20;
    const unsigned PATH_SIZE = 100;

    ExperimentRunner runner(tsp, NUM_RUNS, PATH_SIZE);

    std::vector<std::shared_ptr<Algorithm>> algos = {
        std::make_shared<HybridEvoRepair>(true),
        // std::make_shared<HybridEvoRepair>(false),
        std::make_shared<CustomAlgo>(),
    };

    runner.Run(algos);
}
