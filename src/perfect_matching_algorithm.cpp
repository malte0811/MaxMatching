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

EdgeList PerfectMatchingAlgorithm::find_perfect_matching() {
    auto const& tree_vertices = calculate_matching_or_frustrated_tree();
    if (tree_vertices) {
        throw std::runtime_error("Graph does not have a perfect matching");
    } else {
        return _current_matching.get_matching_edges();
    }
}

std::optional<std::vector<NodeId>> PerfectMatchingAlgorithm::calculate_matching_or_frustrated_tree() {
    while ((_last_root = find_uncovered_vertex())) {
        _tree_for_root.reset(*_last_root);
        _edges_to_check.clear();
        _edges_to_check.emplace_back(*_last_root);
        bool augmented = false;
        std::optional<Edge> next_edge;
        while (not augmented and (next_edge = get_next_edge())) {
            auto const& [end_x, end_y] = *next_edge;
            auto const& repr_x = _tree_for_root.get_representative(end_x);
            auto const& repr_y = _tree_for_root.get_representative(end_y);
            if (repr_x == repr_y) {
                continue;
            }
            assert(_tree_for_root.is_even(repr_x));
            if (_tree_for_root.is_tree_node(repr_y)) {
                if (_tree_for_root.is_even(repr_y)) {
                    auto const& shrunken_odd_nodes = _tree_for_root.shrink_fundamental_circuit(
                            repr_x, repr_y, end_x, end_y
                    );
                    for (auto const& odd_node : shrunken_odd_nodes) {
                        _edges_to_check.emplace_back(odd_node);
                    }
                }
            } else if (_current_matching.is_matched(repr_y)) {
                _tree_for_root.extend(repr_x, repr_y, end_x);
                _edges_to_check.emplace_back(_current_matching.other_end(repr_y).id());
            } else {
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

std::optional<NodeId> PerfectMatchingAlgorithm::find_uncovered_vertex() const {
    auto const& first_guess = _last_root ? *_last_root + 1 : 0;
    for (auto const& [start, end] : {
            std::make_pair(first_guess, _graph.num_nodes()),
            std::make_pair(0U, first_guess),
    }) {
        for (NodeId i = start; i < end; ++i) {
            if (_allowed_vertices.at(i) and not _current_matching.is_matched(Representative(i))) {
                return i;
            }
        }
    }
    return std::nullopt;
}

std::optional<Edge> PerfectMatchingAlgorithm::get_next_edge() {
    while (not _edges_to_check.empty()) {
        auto& partial_node = _edges_to_check.back();
        if (auto const* node_id = std::get_if<NodeId>(&partial_node)) {
            // Unexpanded edge set => expand
            EdgeList edges;
            auto const& node = _graph.node(*node_id);
            edges.reserve(node.degree());
            for (auto const& neighbor : node.neighbors()) {
                if (_allowed_vertices.at(neighbor)) {
                    edges.emplace_back(*node_id, neighbor);
                }
            }
            partial_node = std::move(edges);
        }
        auto* neighbors = std::get_if<EdgeList>(&partial_node);
        if (not neighbors->empty()) {
            auto const next = neighbors->back();
            neighbors->pop_back();
            assert(_allowed_vertices.at(next.first));
            if (_allowed_vertices.at(next.second)) {
                return next;
            }
        } else {
            _edges_to_check.pop_back();
        }
    }
    return std::nullopt;
}

