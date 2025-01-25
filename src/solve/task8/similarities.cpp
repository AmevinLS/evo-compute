#pragma once

#include "../common/types.cpp"
#include <stdexcept>

// Calculates the number of common edges between two solutions
unsigned common_edges_similarity(const solution_t &sol1,
                                 const solution_t &sol2) {

    unsigned num_common_edges = 0;
    for (unsigned i = 0; i < sol1.path.size(); i++) {
        unsigned node1 = sol1.path[i];
        unsigned node2 = sol1.path[sol1.next(i)];
        if (sol2.remaining_nodes.count(node1) +
                sol2.remaining_nodes.count(node2) >
            0)
            continue;

        for (unsigned j = 0; j < sol2.path.size(); j++) {
            unsigned other_node1 = sol2.path[j];
            unsigned other_node2 = sol2.path[sol2.next(j)];
            if ((node1 == other_node1 && node2 == other_node2) ||
                (node1 == other_node2 && node2 == other_node1))
                num_common_edges++;
        }
    }
    return num_common_edges;
}

unsigned common_nodes_similarity(const solution_t &sol1,
                                 const solution_t &sol2) {
    if (sol1.path.size() != sol2.path.size())
        throw std::invalid_argument(
            "Solutions must have paths of the same size");

    unsigned num_common_nodes = 0;
    for (unsigned node : sol1.path) {
        num_common_nodes += (1 - sol2.remaining_nodes.count(node));
    }
    return num_common_nodes;
}
