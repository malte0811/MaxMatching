#include <cassert>
#include <iostream>
#include "perfect_matching_algorithm.h"
#include "alternating_tree.h"

PerfectMatchingAlgorithm::PerfectMatchingAlgorithm(Matching& matching, Graph const& graph, std::vector<char> const& allowed_vertices)
: _current_matching(matching),
  _graph(graph),
  _allowed_vertices(allowed_vertices),
  _tree_for_root(_current_matching, 0) {
    assert(_current_matching.total_num_nodes() == _graph.num_nodes());
    assert(_current_matching.total_num_nodes() == _allowed_vertices.size());
}

EdgeSet PerfectMatchingAlgorithm::find_perfect_matching() {
    auto const& tree_vertices = calc_matching_and_uncovered_root();
    if (tree_vertices) {
        throw std::runtime_error("Graph does not have a perfect matching");
    } else {
        return _current_matching.get_matching_edges();
    }
}

std::optional<std::vector<NodeId>> PerfectMatchingAlgorithm::calc_matching_and_uncovered_root() {
    std::optional<NodeId> next_root = 0;
    while ((next_root = find_uncovered_vertex(*next_root))) {
#ifdef DEBUG_PRINT
        std::cout << "Starting iteration with root " << *next_root << " and matching:\n";
        for (NodeId i = 0; i < _graph.num_nodes(); ++i) {
            if (_current_matching.is_matched(Representative(i))) {
                auto const& other = _current_matching.other_end(Representative(i));
                if (i < other.id()) {
                    std::cout << i << "<->" << other.id() << '\n';
                }
            }
        }
#endif
        _tree_for_root.reset(*next_root);
        _open_edges.clear();
        add_incident_edges(*next_root);
        bool augmented = false;
        while (not augmented and not _open_edges.empty()) {
            auto const& [end_x, end_y] = _open_edges.back();
            _open_edges.pop_back();
            auto const& repr_x = _tree_for_root.get_representative(end_x);
            auto const& repr_y = _tree_for_root.get_representative(end_y);
            if (repr_x == repr_y) {
                continue;
            }
#ifdef DEBUG_PRINT
                std::cout << "Processing edge between representatives " << repr_x.id() << " and " << repr_y.id() << '\n';
#endif
            assert(_tree_for_root.is_even(repr_x));
            if (_tree_for_root.is_tree_node(repr_y)) {
                if (_tree_for_root.is_even(repr_y)) {
                    auto const& shrunken_odd_nodes = _tree_for_root.shrink_fundamental_circuit(
                            repr_x, repr_y, end_x, end_y
                    );
                    for (auto const& odd_node : shrunken_odd_nodes) {
                        add_incident_edges(odd_node);
                    }
                }
            } else if (_current_matching.is_matched(repr_y)) {
#ifdef DEBUG_PRINT
                std::cout << "Extending to " << repr_y.id() << " and matched edge to " << _current_matching.other_end(repr_y).id() << '\n';
#endif
                _tree_for_root.extend(repr_x, repr_y, end_x);
                add_incident_edges(_current_matching.other_end(repr_y).id());
            } else {
#ifdef DEBUG_PRINT
                std::cout << "Augmenting to " << repr_y.id() << "\n";
#endif
                _tree_for_root.augment_and_unshrink(repr_x, end_x, repr_y);
                augmented = true;
            }
        }
        if (not augmented) {
            _tree_for_root.unshrink();
            return _tree_for_root.get_tree_vertices();
        }
    }
    return std::nullopt;
}

std::optional<NodeId> PerfectMatchingAlgorithm::find_uncovered_vertex(NodeId last) const {
    for (auto const& [start, end] : {
            std::make_pair(last + 1, _graph.num_nodes()),
            std::make_pair(0U, last + 1),
    }) {
        for (NodeId i = start; i < end; ++i) {
            if (_allowed_vertices.at(i) and not _current_matching.is_matched(Representative(i))) {
                return i;
            }
        }
    }
    return std::nullopt;
}

void PerfectMatchingAlgorithm::add_incident_edges(NodeId node) {
    for (auto const& graph_neighbor : _graph.node(node).neighbors()) {
        if (_allowed_vertices[graph_neighbor]) {
            _open_edges.emplace_back(node, graph_neighbor);
        }
    }
}

