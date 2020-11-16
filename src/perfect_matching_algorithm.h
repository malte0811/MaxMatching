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
    explicit PerfectMatchingAlgorithm(Matching& matching, Graph const& graph, std::vector<char> const& allowed_vertices);

    [[nodiscard]] EdgeSet find_perfect_matching();

    [[nodiscard]] std::optional<std::vector<NodeId>> calc_matching_and_uncovered_root();

private:
    [[nodiscard]] std::optional<NodeId> find_uncovered_vertex(NodeId last) const;

    void add_incident_edges(NodeId node);

    std::vector<std::pair<NodeId, NodeId>> _open_edges;
    Matching& _current_matching;
    Graph const& _graph;
    std::vector<char> const& _allowed_vertices;
    AlternatingTree _tree_for_root;
};


#endif //MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H
