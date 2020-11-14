#ifndef MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H
#define MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H

#include <vector>
#include <optional>
#include "graph.hpp"
#include "matching.h"
#include "alternating_tree.h"

class PerfectMatchingAlgorithm {
public:
    using EdgeList = std::vector<std::pair<NodeId, NodeId>>;

    explicit PerfectMatchingAlgorithm(Graph const& graph);

    EdgeList find_perfect_matching();
private:
    [[nodiscard]] std::optional<NodeId> find_uncovered_vertex() const;

    void add_incident_edges(NodeId node);

    EdgeList _open_edges;
    Matching _current_matching;
    Graph const& _graph;
};


#endif //MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H
