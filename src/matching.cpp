//
// Created by malte on 11/11/20.
//

#include <numeric>
#include <cassert>
#include "matching.h"

Matching::Matching(NodeId total_nodes) : _matched_vertices(total_nodes), _cardinality(0) {
    std::iota(_matched_vertices.begin(), _matched_vertices.end(), 0);
}

bool Matching::is_matched(NodeId const node) const {
    return _matched_vertices.at(node) != node;
}

bool Matching::contains_edge(NodeId const end_a, NodeId const end_b) const {
    return _matched_vertices.at(end_a) == end_b;
}

void Matching::add_to_matching(NodeId const end_a, NodeId const end_b) {
    assert(not is_matched(end_a));
    assert(not is_matched(end_b));
    std::swap(_matched_vertices.at(end_a), _matched_vertices.at(end_b));
    ++_cardinality;
}

void Matching::augment_along(std::vector<NodeId> const& path) {
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
        _matched_vertices.at(new_end) = fixed_end;
        _matched_vertices.at(fixed_end) = new_end;
    }
    ++_cardinality;
}

std::vector<Matching::Edge> Matching::get_edges() const {
    std::vector<Edge> edges;
    edges.reserve(_cardinality);
    for (NodeId i = 0; i + 1 < _matched_vertices.size(); ++i) {
        auto const& matched_to = _matched_vertices.at(i);
        if (matched_to > i) {
            assert(_matched_vertices.at(matched_to) == i);
            edges.emplace_back(i, matched_to);
        }
    }
    return edges;
}

size_t Matching::size() const {
    return _cardinality;
}
