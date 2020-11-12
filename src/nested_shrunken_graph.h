#ifndef MAXMATCHING_NESTED_SHRUNKEN_GRAPH_H
#define MAXMATCHING_NESTED_SHRUNKEN_GRAPH_H

#include "graph.hpp"
#include "union_find.hpp"

//TODO something like STRONG_TYPEDEF?
using Representative = NodeId;
using VertexSet = std::vector<Representative>;

class NestedShrunkenGraph {
public:

    explicit NestedShrunkenGraph(size_t num_nodes);

    Representative shrink(VertexSet const& to_shrink);

    /**
     * Undo one shrinking operation
     * @return The vertex set shrunken in the undone operation and the representative of the set after the
     * shrinking/before the expansion
     */
    std::pair<VertexSet, Representative> expand();

    [[nodiscard]] Representative get_representative(NodeId node) const;

    [[nodiscard]] bool is_shrunken() const;
private:
    struct ShrinkStep {
        UnionFind old_partition;
        VertexSet shrunken_circuit;
        Representative shrunken_vertex;
    };

    UnionFind _current_partition;
    std::vector<ShrinkStep> _shrink_stack;
};


#endif //MAXMATCHING_NESTED_SHRUNKEN_GRAPH_H
