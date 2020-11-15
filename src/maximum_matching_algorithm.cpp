#include <numeric>
#include <iostream>
#include "maximum_matching_algorithm.h"
#include "perfect_matching_algorithm.h"

MaximumMatchingAlgorithm::MaximumMatchingAlgorithm(Graph&& graph): _graph(std::move(graph)) {}

EdgeSet MaximumMatchingAlgorithm::calc_maximum_matching() {
    EdgeSet matching;
    std::vector<NodeId> to_original_nodes(_graph.num_nodes());
    std::iota(to_original_nodes.begin(), to_original_nodes.end(), 0);
    bool is_maximum = false;
    while (not is_maximum and _graph.num_nodes() > 1) {
        delete_isolated_nodes(to_original_nodes);
#ifdef DEBUG_PRINT
        std::cout << "Remaining vertices: " << _graph.num_nodes() << '\n';
#endif
        PerfectMatchingAlgorithm perfect_alg(_graph);
        auto const& [matched_edges, root_node] = perfect_alg.calc_matching_and_uncovered_root();
        matching.reserve(matching.size() + matched_edges.size());
        for (auto const& [end_a, end_b] : matched_edges) {
            matching.emplace_back(to_original_nodes.at(end_a), to_original_nodes.at(end_b));
        }
        if (root_node) {
            std::vector<bool> should_remove(_graph.num_nodes(), false);
            for (auto const& [end_a, end_b] : matched_edges) {
                should_remove.at(end_a) = true;
                should_remove.at(end_b) = true;
            }
            should_remove.at(*root_node) = true;
            shrink(should_remove, to_original_nodes);
        } else {
            is_maximum = true;
        }
    }
    return matching;
}

void MaximumMatchingAlgorithm::shrink(std::vector<bool> const& to_remove, std::vector<NodeId>& node_map) {
    auto new_to_previous_ids = _graph.delete_nodes(to_remove);
    for (auto& previous_id : new_to_previous_ids) {
        previous_id = node_map.at(previous_id);
    }
    node_map = std::move(new_to_previous_ids);
}

void MaximumMatchingAlgorithm::delete_isolated_nodes(std::vector<NodeId>& node_map) {
    std::vector<bool> should_delete(_graph.num_nodes());
    for (NodeId i = 0; i < _graph.num_nodes(); ++i) {
        should_delete.at(i) = _graph.node(i).degree() == 0;
    }
    shrink(should_delete, node_map);
}
