#include <iostream>
#include <fstream>
#include <chrono>
#include "graph.h"
#include "maximum_matching_algorithm.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Expected exactly one arg\n";
        return 1;
    }
    try {
#ifdef DEBUG_OUTPUT
        auto const& parsing_start = std::chrono::system_clock::now();
#endif
        std::ifstream input(argv[1]);
        auto g = Graph::read_dimacs(input);
#ifdef DEBUG_OUTPUT
        std::cout << "Parsing done\n";
        auto const& parsing_done = std::chrono::system_clock::now();
        auto const& parsing = std::chrono::duration_cast<std::chrono::milliseconds>(parsing_done - parsing_start);
        std::cout << "Parsing time: " << parsing.count() / 1e3 << " s\n";
#endif
        auto const& num_nodes = g.num_nodes();
        MaximumMatchingAlgorithm solver(std::move(g));
        auto const& matching_edges = solver.calc_maximum_matching();
#ifdef DEBUG_OUTPUT
        auto const& end = std::chrono::system_clock::now();
        auto const& matching = std::chrono::duration_cast<std::chrono::milliseconds>(end - parsing_done);
        std::cout << "Matching time: " << matching.count() / 1e3 << " s\n";
#else
        std::cout << "p edge " << num_nodes << " " << matching_edges.size() << '\n';
        for (auto const& [end_a, end_b] : matching_edges) {
            std::cout << "e " << (end_a + 1) << ' ' << (end_b + 1) << '\n';
        }
#endif
        std::cout << std::flush;
    } catch (std::exception const& xcp) {
        std::cerr << "Caught exception: " << xcp.what() << '\n';
        return 1;
    }
}
