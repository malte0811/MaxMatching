#include <numeric>
#include <iostream>
#include <cassert>
#include "maximum_matching_algorithm.h"
#include "perfect_matching_algorithm.h"

MaximumMatchingAlgorithm::MaximumMatchingAlgorithm(Graph&& graph)
: _graph(std::move(graph)),
_current_matching(_graph.num_nodes()),
_allowed(_graph.num_nodes(), true),
_degrees(_graph.num_nodes()){
    for (NodeId i = 0; i < _graph.num_nodes(); ++i) {
        _degrees.at(i) = _graph.node(i).degree();
    }
}

EdgeList MaximumMatchingAlgorithm::calc_maximum_matching() {
    bool is_maximum = false;
    match_leaves();
    delete_isolated_nodes();
    PerfectMatchingAlgorithm perfect_alg(_current_matching, _graph, _allowed);
    while (not is_maximum and _graph.num_nodes() > _num_blocked_nodes + 1) {
        auto const& tree_vertices = perfect_alg.calculate_matching_or_frustrated_tree();
        if (tree_vertices) {
            std::vector<NodeId> isolated_to_remove;
            for (auto const& to_remove : *tree_vertices) {
                assert(_allowed.at(to_remove));
                _allowed.at(to_remove) = false;
                ++_num_blocked_nodes;
                for (auto const& neighbor : _graph.node(to_remove).neighbors()) {
                    auto& degree = _degrees.at(neighbor);
                    --degree;
                    if (degree == 0) {
                        isolated_to_remove.push_back(neighbor);
                    }
                }
            }
            for (auto const& isolated : isolated_to_remove) {
                auto& allowed = _allowed.at(isolated);
                if (allowed) {
                    allowed = false;
                    ++_num_blocked_nodes;
                }
            }
        } else {
            is_maximum = true;
        }
    }
    return _current_matching.get_matching_edges();
}

void MaximumMatchingAlgorithm::delete_isolated_nodes() {
    for (NodeId node = 0; node < _graph.num_nodes(); ++node) {
        if (_allowed.at(node)) {
            bool has_allowed_neighbor = false;
            for (auto const& neighbor : _graph.node(node).neighbors()) {
                if (_allowed.at(neighbor)) {
                    has_allowed_neighbor = true;
                    break;
                }
            }
            if (not has_allowed_neighbor) {
                _allowed.at(node) = false;
                ++_num_blocked_nodes;
            }
        }
    }
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
