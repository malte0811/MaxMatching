#include <cassert>
#include <algorithm>
#include "nested_shrinking.h"

NestedShrinking::NestedShrinking(size_t num_nodes) : _partition(num_nodes), _set_elements(num_nodes) {
    for (NodeId i = 0; i < num_nodes; ++i) {
        Representative repr(i);
        _partition.at(i) = repr;
        _set_elements.at(repr) = {i};
    }
    validate();
}

Representative NestedShrinking::shrink(Representatives const& to_shrink) {
    assert(to_shrink.size() > 1);
    // Find the largest set (this will be used as the name for the result) and the sum of the set sizes (used to
    // reserve vector capacity)
    Representative result_representative = to_shrink.at(0);
    size_t total_size = 0;
    auto largest_set_size = get_size(result_representative);
    for (auto const& set_repr : to_shrink) {
        auto const& size = get_size(set_repr);
        total_size += size;
        if (size > largest_set_size) {
            result_representative = set_repr;
            largest_set_size = size;
        }
    }
    // Perform the union step and store the data needed to undo it
    auto& result_set = _set_elements.at(result_representative);
    result_set.reserve(total_size);
    ShrinkStep shrink_info;
    shrink_info.elements.reserve(to_shrink.size());
    shrink_info.new_name = result_representative;
    for (auto const& set_repr : to_shrink) {
        if (set_repr != result_representative) {
            auto& old_elements = _set_elements.at(set_repr);
            result_set.insert(result_set.end(), old_elements.begin(), old_elements.end());
            for (auto const& element : old_elements) {
                _partition.at(element) = result_representative;
            }
            shrink_info.elements.push_back({set_repr, std::move(old_elements)});
            // old_elements is in an undefined but valid state after moving from it, clearing brings it into a known
            // state
            old_elements.clear();
        } else {
            shrink_info.elements.push_back({set_repr, {}});
        }
    }
    _shrink_stack.push_back(std::move(shrink_info));
    validate();
    return result_representative;
}

Representative NestedShrinking::get_representative(NodeId const node) const {
    return _partition.at(node);
}

bool NestedShrinking::is_shrunken() const {
    return not _shrink_stack.empty();
}

std::pair<Representatives, Representative> NestedShrinking::expand() {
    auto shrink_to_undo = std::move(_shrink_stack.back());
    _shrink_stack.pop_back();
    Representatives shrunken_vertices;
    shrunken_vertices.reserve(shrink_to_undo.elements.size());
    size_t non_main_sizes_sum = 0;
    for (auto&& replacement : shrink_to_undo.elements) {
        if (replacement.old_name != shrink_to_undo.new_name) {
            for (auto const& replaced_node : replacement.affected_nodes) {
                _partition.at(replaced_node) = replacement.old_name;
            }
            non_main_sizes_sum += replacement.affected_nodes.size();
            _set_elements.at(replacement.old_name) = std::move(replacement.affected_nodes);
        }
        shrunken_vertices.push_back(replacement.old_name);
    }
    auto& total_set = _set_elements.at(shrink_to_undo.new_name);
    // New elements are added at the end of the set, so we can just resize to get the correct set back
    total_set.resize(total_set.size() - non_main_sizes_sum);
    validate();
    return {shrunken_vertices, shrink_to_undo.new_name};
}

size_t NestedShrinking::get_size(Representative const& set) const {
    return get_elements(set).size();
}

std::vector<NodeId> const& NestedShrinking::get_elements(Representative const& set) const {
    return _set_elements.at(set);
}

void NestedShrinking::validate() const {
#ifndef NDEBUG
    std::vector<bool> found_node(_partition.size());
    for (NodeId repr = 0; repr < _partition.size(); ++repr) {
        auto const& nodes = get_elements(Representative(repr));
        if (not nodes.empty()) {
            assert(std::find(nodes.begin(), nodes.end(), repr) != nodes.end());
            for (auto const& node_in_set : nodes) {
                assert(not found_node.at(node_in_set));
                found_node.at(node_in_set) = true;
            }
        }
    }
    assert(std::find(found_node.begin(), found_node.end(), false) == found_node.end());
#endif
}
