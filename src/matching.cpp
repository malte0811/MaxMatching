#include <numeric>
#include <cassert>
#include <tuple>
#include "matching.h"

Matching::Matching(NodeId total_nodes) : _matched_vertices(total_nodes), _real_vertex_used_for(total_nodes) {
    for (NodeId i = 0; i < total_nodes; ++i) {
        Representative repr(i);
        _matched_vertices.at(repr) = repr;
        _real_vertex_used_for.at(repr) = i;
    }
}

bool Matching::is_matched(Representative const node) const {
    return _matched_vertices.at(node) != node;
}

bool Matching::contains_edge(Representative const end_a, Representative const end_b) const {
    return _matched_vertices.at(end_a) == end_b;
}

void Matching::add_edge(NodeId end_a, NodeId end_b) {
    Representative repr_a(end_a);
    Representative repr_b(end_b);
    assert(not is_matched(repr_a));
    assert(not is_matched(repr_b));
    _matched_vertices.at(repr_a) = repr_b;
    _matched_vertices.at(repr_b) = repr_a;
    _real_vertex_used_for.at(repr_a) = end_a;
    _real_vertex_used_for.at(repr_b) = end_b;
    validate();
}

void Matching::augment_along(
        std::vector<Representative> const& path, std::vector<std::pair<NodeId, NodeId>> const& edges
) {
    assert(not is_matched(path.front()));
    assert(not is_matched(path.back()));
    assert(path.size() % 2 == 0);
    assert(edges.size() == path.size() - 1);
    //0+++1---2+++3---4+++5 (path)
    //x+0+x-1-x+2+x-3-x+4+x (edges)
    for (size_t i = 0; i < path.size(); i += 2) {
        auto const& new_end = path.at(i);
        auto const& fixed_end = path.at(i + 1);
#ifndef NDEBUG
        if (i + 2 < path.size()) {
            auto const& old_end = path.at(i + 2);
            assert(contains_edge(fixed_end, old_end));
        }
#endif
        match_unchecked(new_end, fixed_end);
        auto const& edge = edges.at(i);
        _real_vertex_used_for.at(new_end) = edge.first;
        _real_vertex_used_for.at(fixed_end) = edge.second;
    }
    validate();
}

void Matching::shrink(RepresentativeSet const& circuit_to_shrink, EdgeList&& circuit_edges, Representative new_name) {
    std::optional<std::pair<Representative, Representative>> edge_to_outside;
    for (size_t i = 0; i < circuit_to_shrink.size(); ++i) {
        auto const& vertex = circuit_to_shrink.at(i);
        auto const& matched_to = _matched_vertices.at(vertex);
        // The node can only be matched to one of two vertices in the circuit: The one right after or right before it
        bool matched_to_node_in_circuit = false;
        for (auto const& offset : {-1, 1}) {
            auto const& index = (i + circuit_to_shrink.size() + offset) % circuit_to_shrink.size();
            if (matched_to == circuit_to_shrink.at(index)) {
                matched_to_node_in_circuit = true;
                break;
            }
        }
        if (not matched_to_node_in_circuit) {
            assert(not edge_to_outside);
            edge_to_outside = {vertex, matched_to};
        } else {
            _matched_vertices.at(vertex) = vertex;
        }
    }
    if (edge_to_outside.has_value()) {
        auto const&[old_attached_to, outside_vertex] = *edge_to_outside;
        if (old_attached_to != new_name) {
            match_unchecked(outside_vertex, new_name);
            _real_vertex_used_for.at(new_name) = _real_vertex_used_for.at(old_attached_to);
            _matched_vertices.at(old_attached_to) = old_attached_to;
        }
    }
    _shrink_data.push_back(std::move(circuit_edges));
    validate();
}

void Matching::expand(
        Representative current_name,
        RepresentativeSet const& expanded_circuit,
        NestedShrinking const& shrinking
) {
    size_t offset = 0;
    auto const circuit_edges = _shrink_data.back();
    _shrink_data.pop_back();
    if (is_matched(current_name)) {
        auto const& matched_to = other_end(current_name);
        auto const& covered_vertex = shrinking.get_representative(_real_vertex_used_for.at(current_name));
        _real_vertex_used_for.at(covered_vertex) = _real_vertex_used_for.at(current_name);
        match_unchecked(matched_to, covered_vertex);
        bool found_offset = false;
        for (size_t i = 0; not found_offset and i < expanded_circuit.size(); ++i) {
            if (covered_vertex == expanded_circuit.at(i)) {
                offset = i;
                found_offset = true;
            }
        }
        assert(found_offset);
    }
    assert(expanded_circuit.size() % 2 == 1);
    for (size_t i = 1; i < expanded_circuit.size(); i += 2) {
        auto const& base_id = (i + offset) % expanded_circuit.size();
        auto const& next_id = (i + offset + 1) % expanded_circuit.size();
        auto const& vertex_a = expanded_circuit.at(base_id);
        auto const& vertex_b = expanded_circuit.at(next_id);
        match_unchecked(vertex_a, vertex_b);
        std::tie(_real_vertex_used_for.at(vertex_a), _real_vertex_used_for.at(vertex_b)) = circuit_edges.at(next_id);
    }
    validate(&shrinking);
}

void Matching::match_unchecked(Representative end_a, Representative end_b) {
    _matched_vertices.at(end_a) = end_b;
    _matched_vertices.at(end_b) = end_a;
}

Representative Matching::other_end(Representative known) const {
    assert(is_matched(known));
    auto const& matched_to = _matched_vertices.at(known);
    assert(_matched_vertices.at(matched_to) == known);
    return matched_to;
}

size_t Matching::total_num_nodes() const {
    return _matched_vertices.size();
}

void Matching::validate([[maybe_unused]]NestedShrinking const* shrinking) const {
#ifndef NDEBUG
    for (NodeId i = 0; i < total_num_nodes(); ++i) {
        Representative repr(i);
        if (is_matched(repr)) {
            auto const& other = _matched_vertices.at(repr);
            assert(_matched_vertices.at(other) == repr);
            if (shrinking) {
                auto const& self_repr = shrinking->get_representative(i);
                assert(self_repr == shrinking->get_representative(_real_vertex_used_for.at(self_repr)));
            }
        }
    }
#endif
}

EdgeList Matching::get_matching_edges() const {
    assert(_shrink_data.empty());
    EdgeList matching_edges;
    for (NodeId i = 0; i < _matched_vertices.size(); ++i) {
        if (is_matched(Representative(i))) {
            auto const& other = other_end(Representative(i));
            if (i < other.id()) {
                matching_edges.push_back({i, other.id()});
            }
        }
    }
    return matching_edges;
}
