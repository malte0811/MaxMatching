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

EdgeSet MaximumMatchingAlgorithm::calc_maximum_matching() {
    bool is_maximum = false;
    delete_isolated_nodes();
    PerfectMatchingAlgorithm perfect_alg(_current_matching, _graph, _allowed);
    while (not is_maximum and _graph.num_nodes() > _num_blocked_nodes + 1) {
#ifdef DEBUG_PRINT
        std::cout << "Remaining vertices: " << (_graph.num_nodes() - _num_blocked_nodes) << '\n';
#endif
        auto const& tree_vertices = perfect_alg.calc_matching_and_uncovered_root();
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
