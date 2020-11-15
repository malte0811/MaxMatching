#include <iostream>
#include <fstream>
#include "graph.h"
#include "maximum_matching_algorithm.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Expected exactly one arg\n";
        return 1;
    }
    for (auto const& seed : {0, 1, 2, 10, 811}) {
        try {
            std::ifstream input(argv[1]);
            auto g = Graph::read_dimacs(input)
                    //.with_extra_all_edge_vertices(20)
                    .shuffle_with_seed(seed);
            auto const& num_nodes = g.num_nodes();
            MaximumMatchingAlgorithm solver(std::move(g));
            auto const& matching_edges = solver.calc_maximum_matching();
            std::cout << "p edge " << num_nodes << " " << matching_edges.size() << '\n';
            //for (auto const& [end_a, end_b] : matching_edges) {
            //    std::cout << "e " << (end_a + 1) << ' ' << (end_b + 1) << '\n';
            //}
            std::cout << std::flush;
        } catch (std::exception const& xcp) {
            std::cerr << "Caught exception: " << xcp.what() << '\n';
            return 1;
        }
    }
}
