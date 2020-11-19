#include <iostream>
#include <cassert>
#include "maximum_matching_algorithm.h"
#include "perfect_matching_algorithm.h"

MaximumMatchingAlgorithm::MaximumMatchingAlgorithm(Graph const& graph)
        : _graph(graph),
          _current_matching(_graph.num_nodes()),
          _allowed(_graph.num_nodes(), true) {}

EdgeList MaximumMatchingAlgorithm::calc_maximum_matching() {
    bool is_maximum = false;
    match_leaves();
    PerfectMatchingAlgorithm perfect_alg(_current_matching, _graph, _allowed);
    while (not is_maximum and _graph.num_nodes() > _num_blocked_nodes + 1) {
        auto const& tree_vertices = perfect_alg.calculate_matching_or_frustrated_tree();
        if (tree_vertices) {
            // Nodes that were part of the tree are not allowed to be used in further trees
            // If any nodes become isolated after removing these
            for (auto const& to_remove : *tree_vertices) {
                assert(_allowed.at(to_remove));
                _allowed.at(to_remove) = false;
                ++_num_blocked_nodes;
            }
        } else {
            is_maximum = true;
        }
    }
    return _current_matching.get_matching_edges();
}

void MaximumMatchingAlgorithm::match_leaves() {
    // Match each node with leaf neighbors to one of those. This can significantly decrease
    // the number of nodes that need to be considered by the main algorithm without 
    // destroying optimality: If a node with leaf neighbors is matched to some other 
    // neighbor in a maximum matching we can always replace that edge with one to a leaf
    for (NodeId i = 0; i < _graph.num_nodes(); ++i) {
        auto const& node = _graph.node(i);
        if (node.degree() != 1) {
            continue;
        }
        _allowed.at(i) = false;
        ++_num_blocked_nodes;
        auto const& neighbor = node.neighbors().front();
        if (_current_matching.is_matched(Representative(neighbor))) {
            continue;
        }
        assert(not _current_matching.is_matched(Representative(i)));
        _current_matching.add_edge(i, neighbor);
        _allowed.at(neighbor) = false;
        ++_num_blocked_nodes;
    }
}
