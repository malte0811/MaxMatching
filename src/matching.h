#ifndef MAXMATCHING_MATCHING_H
#define MAXMATCHING_MATCHING_H


#include <vector>
#include <optional>
#include "graph.h"
#include "nested_shrinking.h"
#include "representative_vector.h"

class Matching {
public:
    explicit Matching(NodeId total_nodes);

    [[nodiscard]] bool is_matched(Representative name) const;

    [[nodiscard]] bool contains_edge(Representative end_a, Representative end_b) const;

    void add_edge(NodeId end_a, NodeId end_b);

    void augment_along(std::vector<Representative> const& path, std::vector<std::pair<NodeId, NodeId>> const& edges);

    /**
     * Shrinks an odd circuit.
     * @param circuit_to_shrink representatives of the vertices of the circuit
     * @param circuit_edges the graph edges of the circuit. Needs to fulfill
     * R(circuit_edges[i+1].first) == R(circuit_edges[i].second) == circuit_to_shrink[i]
     * @param new_name The representative of the shrunken vertex
     */
    void shrink(
            RepresentativeSet const& circuit_to_shrink, EdgeList&& circuit_edges, Representative new_name
    );

    /**
     * Expands an odd circuit previously shrunken using "shrink"
     * @param current_name The representative of the shrunken circuit
     * @param expanded_circuit The vertices of the circuit, in the same order as used to shrink the circuit
     * @param shrinking The shrinking used, with this expansion already done
     */
    void expand(
            Representative current_name, RepresentativeSet const& expanded_circuit, NestedShrinking const& shrinking
    );

    [[nodiscard]] Representative other_end(Representative known_end) const;

    [[nodiscard]] size_t total_num_nodes() const;

    [[nodiscard]] EdgeList get_matching_edges() const;

private:
    void match_unchecked(Representative end_a, Representative end_b);

    // Use pointer instead of std::optional to avoid copies when the function is disabled (in release mode)
    void validate(NestedShrinking const* shrinking = nullptr) const;

    /// Maps a representative either to itself if it is unmatched or the representative it is matched to
    RepresentativeVector<Representative> _matched_vertices;
    /// Maps a representative to the actual vertex used for the incident matching edge
    RepresentativeVector<NodeId> _real_vertex_used_for;
    /// Acts as a stack storing the data needed to undo shrinking operations in addition to the data stored elsewhere
    std::vector<EdgeList> _shrink_data;
};


#endif //MAXMATCHING_MATCHING_H
