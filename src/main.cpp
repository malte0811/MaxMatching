#include <iostream>
#include <fstream>
#include "graph.hpp"
#include "perfect_matching_algorithm.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Expected exactly one arg\n";
        return 1;
    }
    try {
        //TODO adapt to Meike's IO requirements
        std::ifstream input(argv[1]);
        auto const& g = Graph::read_dimacs(input);
        PerfectMatchingAlgorithm solver(g);
        auto const& matching_edges = solver.find_perfect_matching();
        std::cout << "Found " << matching_edges.size() << " edges in graph with " << g.num_nodes() << " nodes\n";
        // for (auto const&[end_a, end_b] : matching_edges) {
        //     std::cout << end_a << ", " << end_b << '\n';
        // }
    } catch (std::exception const& xcp) {
        std::cerr << "Caught exception: " << xcp.what() << '\n';
        return 1;
    }
}
