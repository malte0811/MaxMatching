#ifndef MAXMATCHING_MATCHING_H
#define MAXMATCHING_MATCHING_H


#include <vector>
#include <optional>
#include "graph.hpp"
#include "nested_shrinking.h"

class Matching {
public:
    explicit Matching(NodeId total_nodes);

    [[nodiscard]] bool is_matched(Representative name) const;

    [[nodiscard]] bool contains_edge(Representative end_a, Representative end_b) const;

    void add_to_matching(Representative end_a, Representative end_b);

    void augment_along(std::vector<Representative> const& path, std::vector<std::pair<NodeId, NodeId>> const& edges);

    // R(circuit_edges[i+1].first) == R(circuit_edges[i].second) == circuit_to_shrink[i]
    void shrink(VertexSet const& circuit_to_shrink, EdgeSet&& circuit_edges, Representative new_name);

    void expand(
            Representative current_name,
            VertexSet const& expanded_circuit,
            NestedShrinking const& shrinking
    );

    [[nodiscard]] Representative other_end(Representative known_end) const;

    [[nodiscard]] size_t total_num_nodes() const;

    [[nodiscard]] EdgeSet get_matching_edges() const;

private:
    void match_unchecked(Representative end_a, Representative end_b);

    void validate(std::optional<NestedShrinking> const& shrinking = std::nullopt) const;

    std::vector<Representative> _matched_vertices;
    std::vector<NodeId> _real_vertex_used_for;
    std::vector<EdgeSet> _shrink_data;
};


#endif //MAXMATCHING_MATCHING_H
