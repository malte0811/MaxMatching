#include <numeric>
#include <cassert>
#include <tuple>
#include <iostream>
#include "matching.h"

Matching::Matching(NodeId total_nodes) : _matched_vertices(total_nodes), _real_vertex_used_for(total_nodes) {
    for (NodeId i = 0; i < total_nodes; ++i) {
        _matched_vertices.at(i) = Representative(i);
        _real_vertex_used_for.at(i) = i;
    }
}

bool Matching::is_matched(Representative const node) const {
    return _matched_vertices.at(node.id()) != node;
}

bool Matching::contains_edge(Representative const end_a, Representative const end_b) const {
    return _matched_vertices.at(end_a.id()) == end_b;
}

void Matching::add_to_matching(Representative const end_a, Representative const end_b) {
    assert(not is_matched(end_a));
    assert(not is_matched(end_b));
    std::swap(_matched_vertices.at(end_a.id()), _matched_vertices.at(end_b.id()));
    validate();
}

void Matching::augment_along(std::vector<Representative> const& path, std::vector<std::pair<NodeId, NodeId>> const& edges) {
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
        _real_vertex_used_for.at(new_end.id()) = edge.first;
        _real_vertex_used_for.at(fixed_end.id()) = edge.second;
    }
    validate();
}

void Matching::shrink(VertexSet const& circuit_to_shrink, EdgeSet&& circuit_edges, Representative new_name) {
    // TODO this is a bit awkward
    // Potential replacement: check if equal to circuit.at(+-1)
    std::vector<char> in_circuit(_matched_vertices.size(), 0);
    for (auto const& vertex : circuit_to_shrink) {
        in_circuit.at(vertex.id()) = 1;
    }
    std::optional<std::pair<Representative, Representative>> edge_to_outside;
    for (auto const& vertex : circuit_to_shrink) {
        auto const& matched_to = _matched_vertices.at(vertex.id());
        if (not in_circuit.at(matched_to.id())) {
            assert(not edge_to_outside);
            edge_to_outside = {vertex, matched_to};
        } else {
            // TODO probably a waste of time
            _matched_vertices.at(vertex.id()) = vertex;
        }
    }
    if (edge_to_outside.has_value()) {
        auto const&[old_attached_to, outside_vertex] = *edge_to_outside;
        if (old_attached_to != new_name) {
            match_unchecked(outside_vertex, new_name);
            _real_vertex_used_for.at(new_name.id()) = _real_vertex_used_for.at(old_attached_to.id());
            _matched_vertices.at(old_attached_to.id()) = old_attached_to;
        }
    }
    _shrink_data.push_back(std::move(circuit_edges));
    validate();
}

void Matching::expand(
        Representative current_name,
        VertexSet const& expanded_circuit,
        NestedShrinking const& shrinking
) {
    size_t offset = 0;
    auto const circuit_edges = _shrink_data.back();
    _shrink_data.pop_back();
    if (is_matched(current_name)) {
        auto const& matched_to = other_end(current_name);
        auto const& covered_vertex = shrinking.get_representative(_real_vertex_used_for.at(current_name.id()));
#ifdef DEBUG_PRINT
        std::cout << "Unshrinking: covered vertex is " << covered_vertex.id() << '\n';
#endif
        _real_vertex_used_for.at(covered_vertex.id()) = _real_vertex_used_for.at(current_name.id());
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
        std::tie(_real_vertex_used_for.at(vertex_a.id()), _real_vertex_used_for.at(vertex_b.id())) = circuit_edges.at(next_id);
    }
    validate(&shrinking);
}

void Matching::match_unchecked(Representative end_a, Representative end_b) {
    _matched_vertices.at(end_a.id()) = end_b;
    _matched_vertices.at(end_b.id()) = end_a;
}

Representative Matching::other_end(Representative known) const {
    assert(is_matched(known));
    auto const& matched_to = _matched_vertices.at(known.id());
    assert(_matched_vertices.at(matched_to.id()) == known);
    return matched_to;
}

size_t Matching::total_num_nodes() const {
    return _matched_vertices.size();
}

void Matching::validate([[maybe_unused]]NestedShrinking const* shrinking) const{
#ifndef NDEBUG
    for (NodeId i = 0; i < total_num_nodes(); ++i) {
        if (is_matched(Representative(i))) {
            auto const& other = _matched_vertices.at(i);
            assert(_matched_vertices.at(other.id()).id() == i);
        }
        if (shrinking) {
            auto const& self_repr = shrinking->get_representative(i);
            assert(self_repr == shrinking->get_representative(_real_vertex_used_for.at(self_repr.id())));
        }
    }
#endif
}

EdgeSet Matching::get_matching_edges() const {
    assert(_shrink_data.empty());
    EdgeSet matching_edges;
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
