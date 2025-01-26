#pragma once

#include "random.cpp"
#include "types.cpp"

list_t combine(const solution_t &left, const solution_t &right, bool fill) {
    auto edge_set = left.to_edges();
    std::vector<unsigned int> new_path;
    std::vector<bool> rn = std::vector<bool>(left.tsp->n, true);
    bool prev = false;
    bool curr = false;

    new_path.reserve(left.path.size());

    for (int i = 0; i < right.path.size(); i++) {
        curr = edge_set.count(
                   edge_t{right.path[i], right.path[right.next(i)]}) > 0;

        if (prev || curr) {
            new_path.push_back(right.path[i]);
            rn[right.path[i]] = false;
        } else if (fill) {
            new_path.push_back(UINT_MAX);
        }

        prev = curr;
    }

    if (fill) {
        int idx = random_num(0, rn.size());

        for (int i = 0; i < new_path.size(); i++) {
            if (new_path[i] != UINT_MAX) {
                continue;
            }

            while (!rn[idx]) {
                idx = idx == rn.size() - 1 ? 0 : idx + 1;
            }

            new_path[i] = idx;
            rn[idx] = false;
        }
    }

    return new_path;
}

std::pair<unsigned, unsigned> random_tournament(std::vector<int> &weights) {
    unsigned par1 = random_num(weights);
    weights[par1] = 0;
    unsigned par2 = random_num(weights);
    weights[par1] = 1;
    return {par1, par2};
}
