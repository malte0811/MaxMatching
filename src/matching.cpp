#include <numeric>
#include <cassert>
#include "matching.h"

Matching::Matching(NodeId total_nodes) : _matched_vertices(total_nodes), _cardinality_at_shrinking_step(0) {
    for (NodeId i = 0; i < total_nodes; ++i) {
        _matched_vertices.at(i) = Representative(i);
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
    ++_cardinality_at_shrinking_step;
}

void Matching::augment_along(std::vector<Representative> const& path) {
    assert(not is_matched(path.front()));
    assert(not is_matched(path.back()));
    assert(path.size() % 2 == 0);
    //0+++1---2+++3---4+++5
    for (size_t i = 0; i < path.size(); i += 2) {
        auto const& new_end = path.at(i);
        auto const& fixed_end = path.at(i + 1);
        if (i + 2 < path.size()) {
            auto const& old_end = path.at(i + 2);
            assert(contains_edge(fixed_end, old_end));
        }
        match_unchecked(new_end, fixed_end);
    }
    ++_cardinality_at_shrinking_step;
}

void Matching::shrink(VertexSet const& circuit_to_shrink, Representative new_name) {
    // TODO this is a bit awkward
    // Potential replacement: check if equal to circuit.at(+-1)
    std::vector<char> in_circuit(_matched_vertices.size(), 0);
    for (auto const& vertex : circuit_to_shrink) {
        in_circuit.at(vertex.id()) = 1;
    }
    bool found_external_edge = false;
    for (auto const& vertex : circuit_to_shrink) {
        auto const& matched_to = _matched_vertices.at(vertex.id());
        if (not in_circuit.at(matched_to.id())) {
            assert(not found_external_edge);
            found_external_edge = true;
            match_unchecked(matched_to, new_name);
        } else {
            // TODO probably a waste of time
            _matched_vertices.at(vertex.id()) = vertex;
        }
    }
}

void Matching::expand(
        Representative current_name,
        VertexSet const& expanded_circuit,
        //TODO is this ever empty?
        std::optional<Representative> const& covered_vertex
) {
    size_t offset = 0;
    if (covered_vertex.has_value()) {
        auto const& matched_to = _matched_vertices.at(current_name.id());
        match_unchecked(matched_to, *covered_vertex);
        bool found_offset = false;
        for (size_t i = 0; not found_offset and i < expanded_circuit.size(); ++i) {
            if (*covered_vertex == expanded_circuit.at(i)) {
                offset = i;
                found_offset = true;
            }
        }
        assert(found_offset);
    }
    assert(expanded_circuit.size() % 2 == 1);
    for (size_t i = 1; i < expanded_circuit.size(); i += 2) {
        auto const& vertex_a = expanded_circuit.at((i + offset) % expanded_circuit.size());
        auto const& vertex_b = expanded_circuit.at((i + offset + 1) % expanded_circuit.size());
        match_unchecked(vertex_a, vertex_b);
    }
}

void Matching::match_unchecked(Representative end_a, Representative end_b) {
    _matched_vertices.at(end_a.id()) = end_b;
    _matched_vertices.at(end_b.id()) = end_a;
}

Representative Matching::other_end(Representative known) {
    assert(is_matched(known));
    return _matched_vertices.at(known.id());
}

size_t Matching::total_num_nodes() const {
    return _matched_vertices.size();
}
