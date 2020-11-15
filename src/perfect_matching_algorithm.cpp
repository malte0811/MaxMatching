#include <cassert>
#include <iostream>
#include "perfect_matching_algorithm.h"
#include "alternating_tree.h"

PerfectMatchingAlgorithm::PerfectMatchingAlgorithm(Graph const& graph)
: _current_matching(graph.num_nodes()),
_graph(graph) {}

EdgeSet PerfectMatchingAlgorithm::find_perfect_matching() {
    auto const& [edges, exposed] = calc_matching_and_uncovered_root();
    if (exposed.has_value()) {
        throw std::runtime_error("Graph does not have a perfect matching");
    } else {
        return edges;
    }
}

std::pair<EdgeSet, std::optional<NodeId>> PerfectMatchingAlgorithm::calc_matching_and_uncovered_root() {
    AlternatingTree tree_for_root(_current_matching, 0);
    while (auto const& next_root = find_uncovered_vertex()) {
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
        tree_for_root.reset(*next_root);
        _open_edges.clear();
        add_incident_edges(*next_root);
        bool augmented = false;
        while (not augmented and not _open_edges.empty()) {
            auto const& [end_x, end_y] = _open_edges.back();
            _open_edges.pop_back();
            auto const& repr_x = tree_for_root.get_representative(end_x);
            auto const& repr_y = tree_for_root.get_representative(end_y);
            if (repr_x == repr_y) {
                continue;
            }
#ifdef DEBUG_PRINT
                std::cout << "Processing edge between representatives " << repr_x.id() << " and " << repr_y.id() << '\n';
#endif
            assert(tree_for_root.is_even(repr_x));
            if (tree_for_root.is_tree_node(repr_y)) {
                if (tree_for_root.is_even(repr_y)) {
                    auto const& shrunken_odd_nodes = tree_for_root.shrink_fundamental_circuit(
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
                tree_for_root.extend(repr_x, repr_y, end_x);
                add_incident_edges(_current_matching.other_end(repr_y).id());
            } else {
#ifdef DEBUG_PRINT
                std::cout << "Augmenting to " << repr_y.id() << "\n";
#endif
                tree_for_root.augment_and_unshrink(repr_x, end_x, repr_y);
                augmented = true;
                //std::cout << "Augmented, now " << _current_matching.get_matching_edges().size() << " edges\n";
            }
        }
        if (not augmented) {
            tree_for_root.unshrink();
            return {_current_matching.get_matching_edges(), next_root};
        }
    }
    return {_current_matching.get_matching_edges(), std::nullopt};
}

std::optional<NodeId> PerfectMatchingAlgorithm::find_uncovered_vertex() const {
    //for (NodeId i = _graph.num_nodes() - 1; i + 1 > 0; --i) {
    for (NodeId i = 0; i < _graph.num_nodes(); ++i) {
        if (not _current_matching.is_matched(Representative(i))) {
            return i;
        }
    }
    return std::nullopt;
}

void PerfectMatchingAlgorithm::add_incident_edges(NodeId node) {
    for (auto const& graph_neighbor : _graph.node(node).neighbors()) {
        _open_edges.push_back({node, graph_neighbor});
    }
}

