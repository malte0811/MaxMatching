#include <numeric>
#include <iostream>
#include <cassert>
#include "maximum_matching_algorithm.h"
#include "perfect_matching_algorithm.h"

MaximumMatchingAlgorithm::MaximumMatchingAlgorithm(Graph&& graph): _graph(std::move(graph)) {}

EdgeSet MaximumMatchingAlgorithm::calc_maximum_matching() {
    EdgeSet final_matching;
    EdgeSet temporary_matching;
    std::vector<NodeId> to_original_nodes(_graph.num_nodes());
    std::vector<std::optional<NodeId>> to_current_node(_graph.num_nodes());
    std::iota(to_original_nodes.begin(), to_original_nodes.end(), 0);
    std::iota(to_current_node.begin(), to_current_node.end(), 0);
    bool is_maximum = false;
    while (not is_maximum and _graph.num_nodes() > 1) {
        delete_isolated_nodes(to_original_nodes, to_current_node);
//#ifdef DEBUG_PRINT
        std::cout << "Remaining vertices: " << _graph.num_nodes() << ", matching: " << final_matching.size() << " final and " << temporary_matching.size() << " temporary edges\n";
//#endif
        Matching current_matching(_graph.num_nodes());
        for (auto const& [end_a, end_b] : temporary_matching) {
            current_matching.add_to_matching(
                    Representative(to_current_node.at(end_a).value()),
                    Representative(to_current_node.at(end_b).value())
            );
        }
        PerfectMatchingAlgorithm perfect_alg(std::move(current_matching), _graph);
        auto const& [tree_vertices, new_matching] = perfect_alg.calc_matching_and_uncovered_root();
        auto const& new_matching_edges = new_matching.get_matching_edges();
        if (tree_vertices) {
            std::vector<bool> should_remove(_graph.num_nodes(), false);
            for (auto const& to_remove : *tree_vertices) {
                should_remove.at(to_remove) = true;
            }
            temporary_matching.clear();
            for (auto const& [end_a, end_b] : new_matching_edges) {
                assert(should_remove.at(end_a) == should_remove.at(end_b));
                if (should_remove.at(end_a)) {
                    final_matching.emplace_back(to_original_nodes.at(end_a), to_original_nodes.at(end_b));
                } else {
                    temporary_matching.emplace_back(to_original_nodes.at(end_a), to_original_nodes.at(end_b));
                }
            }
            shrink(should_remove, to_original_nodes, to_current_node);
        } else {
            final_matching.insert(final_matching.end(), new_matching_edges.begin(), new_matching_edges.end());
            is_maximum = true;
        }
    }
    return final_matching;
}

void MaximumMatchingAlgorithm::shrink(std::vector<bool> const& to_remove, std::vector<NodeId>& node_map, std::vector<std::optional<NodeId>>& reverse_node_map) {
    auto new_to_previous_ids = _graph.delete_nodes(to_remove);
    std::fill(reverse_node_map.begin(), reverse_node_map.end(), std::nullopt);
    for (NodeId new_id = 0; new_id < _graph.num_nodes(); ++new_id) {
        auto& entry = new_to_previous_ids.at(new_id);
        entry = node_map.at(entry);
        reverse_node_map.at(entry) = new_id;
    }
    node_map = std::move(new_to_previous_ids);
}

void MaximumMatchingAlgorithm::delete_isolated_nodes(std::vector<NodeId>& node_map, std::vector<std::optional<NodeId>>& reverse_node_map) {
    std::vector<bool> should_delete(_graph.num_nodes());
    size_t num_isolated_vertices = 0;
    std::vector<size_t> degree_order;
    for (NodeId i = 0; i < _graph.num_nodes(); ++i) {
        if (_graph.node(i).degree() == 0) {
            should_delete.at(i) = true;
            ++num_isolated_vertices;
        } else {
            degree_order.push_back(_graph.node(i).degree());
        }
    }
    if (num_isolated_vertices > 0) {
        shrink(should_delete, node_map, reverse_node_map);
    }
}
