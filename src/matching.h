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

    void augment_along(std::vector<Representative> const& path);

    void shrink(VertexSet const& circuit_to_shrink, Representative new_name);

    void expand(
            Representative current_name,
            VertexSet const& expanded_circuit,
            std::optional<Representative> const& covered_vertex
    );

    Representative other_end(Representative known_end);

    [[nodiscard]] size_t total_num_nodes() const;

private:

    void match_unchecked(Representative end_a, Representative end_b);

    std::vector<Representative> _matched_vertices;
    size_t _cardinality_at_shrinking_step;
};


#endif //MAXMATCHING_MATCHING_H
