#ifndef MAXMATCHING_MATCHING_H
#define MAXMATCHING_MATCHING_H


#include <vector>
#include "graph.hpp"

class Matching {
public:
    using Edge = std::pair<NodeId, NodeId>;
    explicit Matching(NodeId total_nodes);

    [[nodiscard]] bool is_matched(NodeId node) const;

    [[nodiscard]] bool contains_edge(NodeId end_a, NodeId end_b) const;

    void add_to_matching(NodeId end_a, NodeId end_b);

    void augment_along(std::vector<NodeId> const& path);

    [[nodiscard]] std::vector<Edge> get_edges() const;

    [[nodiscard]] size_t size() const;
private:
    std::vector<NodeId> _matched_vertices;
    size_t _cardinality;
};


#endif //MAXMATCHING_MATCHING_H
