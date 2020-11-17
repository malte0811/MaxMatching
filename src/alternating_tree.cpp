#include <cassert>
#include <algorithm>
#include <utility>
#include <iostream>
#include <tuple>
#include "alternating_tree.h"

void AlternatingTree::extend(Representative tree_repr, Representative matched_repr, NodeId tree_node) {
    assert(not _needs_reset);
    assert(is_even(tree_repr));
    assert(not is_tree_node(matched_repr));
    assert(get_representative(tree_repr.id()) == tree_repr);
    assert(get_representative(tree_node) == tree_repr);
    assert(get_representative(matched_repr.id()) == matched_repr);
    // Nodes outside the tree are never pseudonodes, so matched_repr.id() == end node of the edge
    set_parent(matched_repr, tree_repr, tree_node, matched_repr.id());
    auto const& matched_end = _current_matching.other_end(matched_repr);
    assert(get_representative(matched_end.id()) == matched_end);
    assert(not is_tree_node(matched_end));
    set_parent(matched_end, matched_repr, matched_repr.id(), matched_end.id());
}

std::vector<NodeId> AlternatingTree::shrink_fundamental_circuit(Representative repr_a, Representative repr_b, NodeId node_a, NodeId node_b) {
    assert(not _needs_reset);
    assert(get_representative(repr_a.id()) == repr_a);
    assert(get_representative(repr_b.id()) == repr_b);
    assert(is_even(repr_a));
    assert(is_even(repr_b));
    auto const& fundamental_cycle = find_fundamental_circuit(repr_a, repr_b, node_a, node_b);
    auto const top_node = fundamental_cycle.path_containing_top_node.back().repr;
    auto [cycle_edges, cycle_vertices] = fundamental_cycle.to_edges_and_reprs();

    // Extract odd vertices, do this before overwriting the node states to allow the assertion to work
    std::vector<NodeId> odd_nodes;
    odd_nodes.reserve(cycle_vertices.size() / 2);
    for (size_t i = 1; i < cycle_vertices.size(); i += 2) {
        auto const& node = cycle_vertices.at(i);
        assert(not is_even(node));
        odd_nodes.push_back(node.id());
    }
    assert(odd_nodes.size() == cycle_vertices.size() / 2);

    assert(is_even(top_node));
    auto const top_state = get_state(top_node);
    auto const& top_parent = _parent_node.at(top_node);
    auto const& shrunken_node = _shrinking.shrink(cycle_vertices);
#ifndef NDEBUG
    for (auto const&[end_a, end_b] : cycle_edges) {
        assert(end_a != invalid_node);
        assert(end_b != invalid_node);
    }
#endif
    _current_matching.shrink(cycle_vertices, std::move(cycle_edges), shrunken_node);
    _parent_node.at(shrunken_node) = top_parent;
    // Set the correct node states
    for (Representative node : cycle_vertices) {
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
    assert(not _needs_reset);
    assert(get_representative(tree_repr.id()) == tree_repr);
    assert(get_representative(neighbor.id()) == neighbor);
    assert(not _current_matching.is_matched(neighbor));

    std::vector<Representative> path_to_root{neighbor, tree_repr};
    EdgeList path_edges{{neighbor.id(), tree_node}};
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
    assert(not _needs_reset);
    // There's no need to restore all data structures here, as the tree needs to be reset for the algorithm anyway
    while (_shrinking.is_shrunken()) {
        auto const&[odd_cycle, pseudo_node] = _shrinking.expand();
        _current_matching.expand(pseudo_node, odd_cycle, _shrinking);
    }
    _needs_reset = true;
}

AlternatingTree::NodeStatus AlternatingTree::get_state(Representative node) const {
    assert(not _needs_reset);
    auto const& result = _node_states.at(node.id());
    assert(result != not_representative);
    return result;
}

bool AlternatingTree::is_even(Representative node) const {
    assert(not _needs_reset);
    auto const& state = get_state(node);
    assert(state != not_in_tree);
    return state == root or state == even;
}

void AlternatingTree::set_state(Representative node, NodeStatus new_status) {
    _node_states.at(node.id()) = new_status;
}

Representative AlternatingTree::get_parent_repr(Representative node) const {
    assert(not _needs_reset);
    return _shrinking.get_representative(_parent_node.at(node).edge_end_parent);
}

//TODO node.id() always == link?
void AlternatingTree::set_parent(Representative node, Representative parent_rep, NodeId parent, NodeId link) {
    assert(not _needs_reset);
    assert(is_tree_node(parent_rep));
    assert(not is_tree_node(node));
    _parent_node.at(node) = {link, parent};
    _tree_vertices.push_back(node.id());
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
          _node_states(_current_matching.total_num_nodes()),
          _a_path_index(_parent_node.size(), invalid_id),
          _b_path_index(_parent_node.size(), invalid_id) {
    reset(root_node);
}

Representative AlternatingTree::get_representative(NodeId node) const {
    return _shrinking.get_representative(node);
}

bool AlternatingTree::is_tree_node(Representative node) const {
    assert(not _needs_reset);
    return get_state(node) != not_in_tree;
}

std::pair<NodeId, NodeId> AlternatingTree::get_edge_to_parent(Representative node) const {
    assert(not _needs_reset);
    auto const& parent = _parent_node.at(node);
    return {parent.edge_end_here, parent.edge_end_parent};
}

void AlternatingTree::reset(NodeId root_node) {
    assert(not _shrinking.is_shrunken());
    for (auto const& vertex : _tree_vertices) {
        _parent_node.at(Representative(vertex)) = Parent{0, 0};
        _node_states.at(vertex) = not_in_tree;
    }
    _node_states.at(root_node) = root;
    _tree_vertices.clear();
    _tree_vertices.push_back(root_node);
    _needs_reset = false;
}

std::vector<NodeId> AlternatingTree::get_tree_vertices() const {
    return _tree_vertices;
}

AlternatingTree::FundamentalCircuit AlternatingTree::find_fundamental_circuit(
        Representative repr_a, Representative repr_b, NodeId node_a, NodeId node_b
) const {
    //TODO speed up by using depth data?
    std::vector<FundamentalCircuit::NodeInfo> a_path{{repr_a, node_a, invalid_node}};
    std::vector<FundamentalCircuit::NodeInfo> b_path{{repr_b, node_b, invalid_node}};
    _a_path_index.at(repr_a) = _b_path_index.at(repr_b) = 0;
    while (true) {
        for (auto&[path, indices, other_path, other_indices] : {
                std::tie(a_path, _a_path_index, b_path, _b_path_index),
                std::tie(b_path, _b_path_index, a_path, _a_path_index)
        }) {
            auto& last = path.back();
            if (not is_root(last.repr)) {
                auto const& next = get_parent_repr(last.repr);
                auto const&[vertex_here, vertex_next] = get_edge_to_parent(last.repr);
                last.above = vertex_here;
                if (other_indices.at(next) != invalid_id) {
                    FundamentalCircuit result;
                    result.other_vertex_used_at_top = vertex_next;
                    auto const& other_length = other_indices.at(next) + 1;
                    for (auto&[reset_path, reset_indices] : {
                            std::tie(path, indices),
                            std::tie(other_path, other_indices)
                    }) {
                        for (auto const& id : reset_path) {
                            reset_indices.at(id.repr) = invalid_id;
                        }
                    }
                    other_path.resize(other_length);
                    other_path.back().above = invalid_node;
                    result.path_containing_top_node = std::move(other_path);
                    result.path_without_top_node = std::move(path);
                    return result;
                } else {
                    indices.at(next) = path.size();
                    path.push_back({next, vertex_next, invalid_node});
                }
            }
        }
    }
}

std::pair<EdgeList, RepresentativeSet> AlternatingTree::FundamentalCircuit::to_edges_and_reprs() const {
    RepresentativeSet cycle_reprs;
    EdgeList cycle_edges{{invalid_node, invalid_node}};
    auto const& cycle_length = path_without_top_node.size() +
                               path_containing_top_node.size();
    cycle_reprs.reserve(cycle_length);
    cycle_edges.reserve(cycle_length);
    // Go up path containing the top node
    for (auto const& element : path_containing_top_node) {
        cycle_reprs.push_back(element.repr);
        cycle_edges.back().second = element.below;
        if (element.above != invalid_node) {
            cycle_edges.emplace_back(element.above, invalid_node);
        }
    }
    // Go down the other path
    cycle_edges.emplace_back(other_vertex_used_at_top, invalid_node);
    for (auto it = path_without_top_node.crbegin(); it != path_without_top_node.crend(); ++it) {
        auto const& element = *it;
        cycle_reprs.push_back(element.repr);
        cycle_edges.back().second = element.above;
        if (element.below != invalid_node) {
            cycle_edges.emplace_back(element.below, invalid_node);
        }
    }
    cycle_edges.front().first = cycle_edges.back().first;
    cycle_edges.pop_back();

    assert(cycle_edges.size() == cycle_reprs.size());
    return {cycle_edges, cycle_reprs};
}
