#ifndef MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H
#define MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H

#include <vector>
#include <optional>
#include "graph.h"
#include "matching.h"
#include "alternating_tree.h"

class PerfectMatchingAlgorithm {
public:
    explicit PerfectMatchingAlgorithm(Graph const& graph);

    [[nodiscard]] EdgeSet find_perfect_matching();

    [[nodiscard]] std::pair<EdgeSet, std::optional<NodeId>> calc_matching_and_uncovered_root();
private:
    [[nodiscard]] std::optional<NodeId> find_uncovered_vertex() const;

    void add_incident_edges(NodeId node);

    EdgeSet _open_edges;
    Matching _current_matching;
    Graph const& _graph;
};


#endif //MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H
