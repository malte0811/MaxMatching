#ifndef MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H
#define MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H

#include <vector>
#include <optional>
#include <deque>
#include "graph.h"
#include "matching.h"
#include "alternating_tree.h"

class PerfectMatchingAlgorithm {
public:
    explicit PerfectMatchingAlgorithm(Matching&& matching, Graph const& graph);

    [[nodiscard]] EdgeSet find_perfect_matching();

    [[nodiscard]] std::pair<std::optional<std::vector<NodeId>>, Matching> calc_matching_and_uncovered_root();
private:
    [[nodiscard]] std::optional<NodeId> find_uncovered_vertex() const;

    void add_incident_edges(NodeId node);

    std::deque<std::pair<NodeId, NodeId>> _open_edges;
    //TODO ref?
    Matching _current_matching;
    Graph const& _graph;
};


#endif //MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H
