#include <cassert>
#include <algorithm>
#include <utility>
#include <numeric>
#include "alternating_tree.h"

void AlternatingTree::extend(Representative tree_node, Representative matched_neighbor) {
    assert(is_even(tree_node));
    // Nodes outside the tree are never pseudonodes
    set_parent(matched_neighbor, tree_node);
    auto const& matched_end = _current_matching.other_end(matched_neighbor);
    set_parent(matched_end, matched_neighbor);
}

std::vector<NodeId> AlternatingTree::shrink_fundamental_circuit(Representative end_a, Representative end_b) {
    //TODO rewrite all of this mess, or at least separate it into various bits of mess
    //TODO speed up by using depth data?
    auto const& invalid_id = std::numeric_limits<size_t>::max();
    //TODO reduce code duplication!!!
    std::vector<size_t> a_path_index(_parent_node.size(), invalid_id);
    std::vector<size_t> b_path_index(_parent_node.size(), invalid_id);
    std::vector<Representative> a_path{end_a};
    std::vector<Representative> b_path{end_b};
    a_path_index.at(end_a.id()) = b_path_index.at(end_b.id()) = 0;
    while (a_path_index.at(b_path.back().id()) == invalid_id and b_path_index.at(a_path.back().id()) == invalid_id) {
        if (not is_root(a_path.back())) {
            auto const& next = get_parent(a_path.back());
            a_path_index.at(next.id()) = a_path.size();
            a_path.push_back(next);
        }
        if (not is_root(b_path.back())) {
            auto const& next = get_parent(b_path.back());
            b_path_index.at(next.id()) = b_path.size();
            b_path.push_back(next);
        }
    }
    std::vector<Representative> odd_cycle;
    Representative top_node(0);
    if (a_path_index.at(b_path.back().id()) != invalid_id) {
        // b-path hit the middle of the a-path
        top_node = b_path.back();
        auto const& a_size = a_path_index.at(top_node.id());
        odd_cycle = std::move(b_path);
        odd_cycle.reserve(odd_cycle.size() + a_size);
        for (size_t i = a_size - 1; i + 1 > 0; --i) {
            odd_cycle.push_back(a_path.at(i));
        }
        for (size_t i = a_size; i < a_path.size(); ++i) {
            a_path_index.at(a_path.at(i).id()) = invalid_id;
        }
    } else {
        // a-path hit the middle of the b-path
        top_node = a_path.back();
        auto const& b_size = b_path_index.at(a_path.back().id());
        odd_cycle = std::move(a_path);
        odd_cycle.reserve(odd_cycle.size() + b_size);
        for (size_t i = b_size - 1; i + 1 > 0; --i) {
            odd_cycle.push_back(b_path.at(i));
        }
        for (size_t i = b_size; i < b_path.size(); ++i) {
            b_path_index.at(b_path.at(i).id()) = invalid_id;
        }
    }
    //TODO clean up
    std::vector<NodeId> odd_nodes;
    odd_nodes.reserve(odd_cycle.size() / 2);
    for (auto const& node : odd_cycle) {
        if (not is_even(node)) {
            odd_nodes.push_back(node.id());
        }
    }
    std::vector<std::vector<Representative>> neighbors_in_tree;
    neighbors_in_tree.reserve(odd_cycle.size());
    for (Representative node : odd_cycle) {
        auto const& children = _children.at(node.id());
        std::vector<Representative> node_neighbors;
        node_neighbors.reserve(children.size());
        if (node == top_node and not is_root(top_node)) {
            node_neighbors.push_back(get_parent(top_node));
        }
        for (auto const& child : children) {
            auto const& child_repr = _shrinking.get_representative(child);
            if (a_path_index.at(child_repr.id()) == invalid_id and b_path_index.at(child_repr.id()) == invalid_id) {
                node_neighbors.push_back(child_repr);
            }
        }
        neighbors_in_tree.push_back(std::move(node_neighbors));
    }
    _shrink_stack.push_back({std::move(neighbors_in_tree)});
    auto const& shrunken_node = _shrinking.shrink(odd_cycle);
    _current_matching.shrink(odd_cycle, shrunken_node);
    for (Representative node : odd_cycle) {
        if (node != shrunken_node) {
            set_state(node, not_representative);
        }
    }
    return odd_nodes;
}

Matching&& AlternatingTree::augment_and_unshrink(Representative tree_node, Representative neighbor) {
    assert(not _current_matching.is_matched(neighbor));
    std::vector<Representative> path_to_root{neighbor, tree_node};
    while (not is_root(path_to_root.back())) {
        path_to_root.push_back(get_parent(path_to_root.back()));
    }
    _current_matching.augment_along(path_to_root);
    while (_shrinking.is_shrunken()) {
        auto const&[odd_cycle, pseudo_node] = _shrinking.expand();
        auto const& shrink_data = std::move(_shrink_stack.back());
        _shrink_stack.pop_back();
        std::optional<Representative> matched_node;
        if (_current_matching.is_matched(pseudo_node)) {
            auto const& circuit_mate = _current_matching.other_end(pseudo_node);
            for (size_t i = 0; i < odd_cycle.size(); ++i) {
                auto const& neighbors = shrink_data.circuit_neighbors_in_tree.at(i);
                auto const& neighbor_index = std::find(neighbors.begin(), neighbors.end(), circuit_mate);
                if (neighbor_index != neighbors.end()) {
                    matched_node = odd_cycle.at(i);
                    break;
                }
            }
            assert(matched_node.has_value());
        }
        _current_matching.expand(pseudo_node, odd_cycle, matched_node);
    }
    return std::move(_current_matching);
}

AlternatingTree::NodeStatus AlternatingTree::get_state(Representative node) const {
    auto const& result = _node_states.at(node.id());
    assert(result != not_representative);
    return result;
}

bool AlternatingTree::is_even(Representative node) const {
    auto const& state = get_state(node);
    assert(state != not_in_tree);
    return state == root or state == even;
}

void AlternatingTree::set_state(Representative node, NodeStatus new_status) {
    _node_states.at(node.id()) = new_status;
}

Representative AlternatingTree::get_parent(Representative node) const {
    return _shrinking.get_representative(_parent_node.at(node.id()));
}

void AlternatingTree::set_parent(Representative node, Representative parent) {
    _children.at(parent.id()).push_back(node.id());
    _parent_node.at(node.id()) = parent.id();
    if (is_even(parent)) {
        set_state(node, odd);
    } else {
        set_state(node, even);
    }
}

bool AlternatingTree::is_root(Representative node) const {
    return get_state(node) == root;
}

AlternatingTree::AlternatingTree(Matching matching, NodeId root_node)
        : _current_matching(std::move(matching)),
          _shrinking(_current_matching.total_num_nodes()),
          _parent_node(_current_matching.total_num_nodes()),
          _children(_current_matching.total_num_nodes()),
          _node_states(_current_matching.total_num_nodes(), not_in_tree) {
    std::iota(_parent_node.begin(), _parent_node.end(), 0);
    _node_states.at(root_node) = root;
}

Representative AlternatingTree::get_representative(NodeId node) const {
    return _shrinking.get_representative(node);
}

bool AlternatingTree::is_tree_edge(Representative node_a, Representative node_b) const {
    return get_parent(node_a) == node_b or get_parent(node_b) == node_a;
}

bool AlternatingTree::is_tree_node(Representative node) const {
    return get_state(node) != not_in_tree;
}
