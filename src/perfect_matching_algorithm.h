#ifndef MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H
#define MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H

#include <vector>
#include <optional>
#include <deque>
#include <variant>
#include "graph.h"
#include "matching.h"
#include "alternating_tree.h"

class PerfectMatchingAlgorithm {
public:
    explicit PerfectMatchingAlgorithm(Matching& matching, Graph const& graph, std::vector<char> const& allowed_vertices);

    [[nodiscard]] EdgeList find_perfect_matching();

    [[nodiscard]] std::optional<std::vector<NodeId>> calculate_matching_or_frustrated_tree();

private:
    [[nodiscard]] std::optional<NodeId> find_uncovered_vertex() const;

    [[nodiscard]] std::optional<Edge> get_next_edge();

    std::optional<NodeId> _last_root;
    /**
     * Getting all neighbors of a vertex is relatively expensive, but DFS seems to be the best search order. So store
     * the node ID and only expand it to a list of nodes when we actually need it.
     */
    std::vector<std::variant<EdgeList, NodeId>> _edges_to_check;
    Matching& _current_matching;
    Graph const& _graph;
    std::vector<char> const& _allowed_vertices;
    AlternatingTree _tree_for_root;
};


#endif //MAXMATCHING_PERFECT_MATCHING_ALGORITHM_H
