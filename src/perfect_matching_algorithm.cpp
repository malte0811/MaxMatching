#include <cassert>
#include "perfect_matching_algorithm.h"
#include "alternating_tree.h"

PerfectMatchingAlgorithm::PerfectMatchingAlgorithm(Graph const& graph)
: _current_matching(graph.num_nodes()),
_graph(graph) {}

PerfectMatchingAlgorithm::EdgeList PerfectMatchingAlgorithm::find_perfect_matching() {
    while (auto const& next_root = find_uncovered_vertex()) {
        AlternatingTree tree_for_root(_current_matching, *next_root);
        add_non_tree_edges(*next_root, tree_for_root);
        bool augmented = false;
        while (not augmented and not _open_edges.empty()) {
            auto const& [end_x, end_y] = _open_edges.back();
            _open_edges.pop_back();
            auto const& repr_x = tree_for_root.get_representative(end_x);
            auto const& repr_y = tree_for_root.get_representative(end_y);
            if (repr_x == repr_y) {
                continue;
            }
            assert(tree_for_root.is_even(repr_x));
            if (tree_for_root.is_tree_node(repr_y)) {
                assert(tree_for_root.is_even(repr_y));
                auto const& shrunken_odd_nodes = tree_for_root.shrink_fundamental_circuit(repr_x, repr_y);
                for (auto const& odd_node : shrunken_odd_nodes) {
                    add_non_tree_edges(odd_node, tree_for_root);
                }
            } else if (_current_matching.is_matched(repr_y)) {
                tree_for_root.extend(repr_x, repr_y);
                add_non_tree_edges(_current_matching.other_end(repr_y).id(), tree_for_root);
            } else {
                _current_matching = tree_for_root.augment_and_unshrink(repr_x, repr_y);
                augmented = true;
            }
        }
        if (not augmented) {
            throw std::runtime_error("Graph does not have perfect matching!");
        }
    }
    EdgeList result;
    for (NodeId i = 0; i < _graph.num_nodes(); ++i) {
        auto const& other = _current_matching.other_end(Representative(i));
        if (i < other.id()) {
            result.push_back({i, other.id()});
        }
    }
    return result;
}

std::optional<NodeId> PerfectMatchingAlgorithm::find_uncovered_vertex() const {
    for (NodeId i = 0; i < _graph.num_nodes(); ++i) {
        if (not _current_matching.is_matched(Representative(i))) {
            return i;
        }
    }
    return std::nullopt;
}

void PerfectMatchingAlgorithm::add_non_tree_edges(NodeId node, AlternatingTree const& tree) {
    auto const& node_repr = tree.get_representative(node);
    for (auto const& graph_neighbor : _graph.node(node).neighbors()) {
        auto const& repr = tree.get_representative(graph_neighbor);
        if (not tree.is_tree_edge(repr, node_repr)) {
            _open_edges.push_back({node, graph_neighbor});
        }
    }
}

