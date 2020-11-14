#include <cassert>
#include <algorithm>
#include <utility>
#include <numeric>
#include <iostream>
#include <tuple>
#include "alternating_tree.h"

void AlternatingTree::extend(Representative tree_repr, Representative matched_repr, NodeId tree_node) {
    assert(is_even(tree_repr));
    assert(not is_tree_node(matched_repr));
    assert(get_representative(tree_repr.id()) == tree_repr);
    assert(get_representative(matched_repr.id()) == matched_repr);
    // Nodes outside the tree are never pseudonodes
    set_parent(matched_repr, tree_repr, tree_node, matched_repr.id());
    auto const& matched_end = _current_matching.other_end(matched_repr);
    assert(get_representative(matched_end.id()) == matched_end);
    assert(not is_tree_node(matched_end));
    set_parent(matched_end, matched_repr, matched_repr.id(), matched_end.id());
}

std::vector<NodeId> AlternatingTree::shrink_fundamental_circuit(Representative repr_a, Representative repr_b, NodeId node_a, NodeId node_b) {
    assert(get_representative(repr_a.id()) == repr_a);
    assert(get_representative(repr_b.id()) == repr_b);
    //TODO rewrite all of this mess, or at least separate it into various bits of mess
    //TODO speed up by using depth data?
    auto const& invalid_id = std::numeric_limits<size_t>::max();
    //TODO reduce code duplication!!!
    std::vector<size_t> a_path_index(_parent_node.size(), invalid_id);
    std::vector<size_t> b_path_index(_parent_node.size(), invalid_id);
    struct NodeInfo {
        Representative repr;
        NodeId below;
        NodeId above;
    };
    auto const& invalid_node = std::numeric_limits<NodeId>::max();
    /*              \
     * longer     /  \ shorter
     *           /
     *          /
     */
    std::vector<NodeInfo> shorter_path;
    std::vector<NodeInfo> longer_path;
    NodeId longer_top_node;
    {
        std::vector<NodeInfo> a_path{{repr_a, node_a, invalid_node}};
        std::vector<NodeInfo> b_path{{repr_b, node_b, invalid_node}};
        a_path_index.at(repr_a.id()) = b_path_index.at(repr_b.id()) = 0;
        bool done = false;
        while (not done) {
            for (auto&[path, indices, other_path, other_indices] : {
                    std::tie(a_path, a_path_index, b_path, b_path_index),
                    std::tie(b_path, b_path_index, a_path, a_path_index)
            }) {
                auto& last = path.back();
                if (not is_root(last.repr)) {
                    auto const& next = get_parent_repr(last.repr);
                    auto const&[vertex_here, vertex_next] = get_edge_to_parent(last.repr);
                    last.above = vertex_here;
                    if (other_indices.at(next.id()) != invalid_id) {
                        longer_top_node = vertex_next;
                        other_path.resize(other_indices.at(next.id()) + 1);
                        other_path.back().above = invalid_id;
                        shorter_path = std::move(other_path);
                        longer_path = std::move(path);
                        done = true;
                        break;
                    } else {
                        indices.at(next.id()) = path.size();
                        path.push_back({next, vertex_next, invalid_node});
                    }
                }
            }
        }
    }
    auto const top_node = shorter_path.back().repr;
    std::vector<Representative> odd_cycle;
    EdgeSet cycle_edges{{invalid_node, invalid_node}};
    auto const& cycle_length = shorter_path.size() + longer_path.size();
    odd_cycle.reserve(cycle_length);
    cycle_edges.reserve(cycle_length);
    //Go up the short path
    for (auto const& element : shorter_path) {
        odd_cycle.push_back(element.repr);
        cycle_edges.back().second = element.below;
        //TODO <=> top node on path
        if (element.above != invalid_node) {
            cycle_edges.emplace_back(element.above, invalid_node);
        }
    }
    //Go down the long path
    cycle_edges.emplace_back(longer_top_node, invalid_node);
    for (auto it = longer_path.crbegin(); it != longer_path.crend(); ++it) {
        auto const& element = *it;
        odd_cycle.push_back(element.repr);
        cycle_edges.back().second = element.above;
        //TODO <=> lowest node on path
        if (element.below != invalid_node) {
            cycle_edges.emplace_back(element.below, invalid_node);
        }
    }
    cycle_edges.front().first = cycle_edges.back().first;
    cycle_edges.pop_back();


    assert(cycle_edges.size() == odd_cycle.size());
    //TODO clean up
    std::vector<NodeId> odd_nodes;
    odd_nodes.reserve(odd_cycle.size() / 2);
#ifdef DEBUG_PRINT
    std::cout << "Shrinking ";
    for (auto const& node : odd_cycle) {
        std::cout << node.id() << " ";
    }
    std::cout << '\n';
#endif
    for (auto const& node : odd_cycle) {
        if (not is_even(node)) {
            odd_nodes.push_back(node.id());
        }
    }
    auto const top_state = get_state(top_node);
    auto const& top_parent = _parent_node.at(top_node.id());
    auto const& shrunken_node = _shrinking.shrink(odd_cycle);
#ifndef NDEBUG
    for (auto const&[end_a, end_b] : cycle_edges) {
        assert(end_a != invalid_node);
        assert(end_b != invalid_node);
    }
#endif
    _current_matching.shrink(odd_cycle, std::move(cycle_edges), shrunken_node);
    auto& shrunken_parent = _parent_node.at(shrunken_node.id());
    _shrink_stack.push_back({top_node, shrunken_parent});
    shrunken_parent = top_parent;
    assert(std::find(odd_cycle.begin(), odd_cycle.end(), shrunken_node) != odd_cycle.end());
    for (Representative node : odd_cycle) {
        if (node != shrunken_node) {
            assert(top_state == root or get_state(node) != root);
            set_state(node, not_representative);
        } else {
            set_state(node, top_state);
        }
    }
    return odd_nodes;
}

void AlternatingTree::augment_and_unshrink(Representative tree_repr, NodeId tree_node, Representative neighbor) {
    assert(get_representative(tree_repr.id()) == tree_repr);
    assert(get_representative(neighbor.id()) == neighbor);
    assert(not _current_matching.is_matched(neighbor));
    std::vector<Representative> path_to_root{neighbor, tree_repr};
    EdgeSet path_edges{{neighbor.id(), tree_node}};
    while (not is_root(path_to_root.back())) {
        path_edges.push_back(get_edge_to_parent(path_to_root.back()));
        auto const& next_node = get_parent_repr(path_to_root.back());
        assert(std::find(path_to_root.begin(), path_to_root.end(), next_node) == path_to_root.end());
        path_to_root.push_back(next_node);
    }
    _current_matching.augment_along(path_to_root, path_edges);
    unshrink();
}

void AlternatingTree::unshrink() {
    while (_shrinking.is_shrunken()) {
        auto const&[odd_cycle, pseudo_node] = _shrinking.expand();
        auto const& shrink_data = _shrink_stack.back();
        _shrink_stack.pop_back();
        auto& shrunk_parent = _parent_node.at(pseudo_node.id());
        _parent_node.at(shrink_data.top_node.id()) = shrunk_parent;
        shrunk_parent = shrink_data.old_shrink_parent;

#ifdef DEBUG_PRINT
        std::cout << "Expanding cycle ";
        for (auto const& node : odd_cycle) {
            std::cout << node.id() << " ";
        }
        std::cout << "from " << pseudo_node.id() << '\n';
#endif
        _current_matching.expand(pseudo_node, odd_cycle, _shrinking);
    }
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

Representative AlternatingTree::get_parent_repr(Representative node) const {
    return _shrinking.get_representative(_parent_node.at(node.id()).edge_end_parent);
}

void AlternatingTree::set_parent(Representative node, Representative parent_rep, NodeId parent, NodeId link) {
    assert(is_tree_node(parent_rep));
    assert(not is_tree_node(node));
    _parent_node.at(node.id()) = {link, parent};
    if (is_even(parent_rep)) {
        set_state(node, odd);
    } else {
        set_state(node, even);
    }
}

bool AlternatingTree::is_root(Representative node) const {
    return get_state(node) == root;
}

AlternatingTree::AlternatingTree(Matching& matching, NodeId root_node)
        : _current_matching(matching),
          _shrinking(_current_matching.total_num_nodes()),
          _parent_node(_current_matching.total_num_nodes()),
          _node_states(_current_matching.total_num_nodes()) {
    reset(root_node);
}

Representative AlternatingTree::get_representative(NodeId node) const {
    return _shrinking.get_representative(node);
}

bool AlternatingTree::is_tree_node(Representative node) const {
    return get_state(node) != not_in_tree;
}

std::pair<NodeId, NodeId> AlternatingTree::get_edge_to_parent(Representative node) const {
    auto const& parent = _parent_node.at(node.id());
    return {parent.edge_end_here, parent.edge_end_parent};
}

void AlternatingTree::reset(NodeId root_node) {
    assert(not _shrinking.is_shrunken());
    std::fill(_parent_node.begin(), _parent_node.end(), Parent{0, 0});
    std::fill(_node_states.begin(), _node_states.end(), not_in_tree);
    _node_states.at(root_node) = root;
}
