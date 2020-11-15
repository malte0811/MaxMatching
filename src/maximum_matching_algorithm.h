#ifndef MAXMATCHING_MAXIMUM_MATCHING_ALGORITHM_H
#define MAXMATCHING_MAXIMUM_MATCHING_ALGORITHM_H


#include "graph.h"

class MaximumMatchingAlgorithm {
public:
    explicit MaximumMatchingAlgorithm(Graph&& graph);

    EdgeSet calc_maximum_matching();
private:
    void shrink(std::vector<bool> const& to_remove, std::vector<NodeId>& node_map, std::vector<std::optional<NodeId>>& reverse_node_map);

    void delete_isolated_nodes(std::vector<NodeId>& node_map, std::vector<std::optional<NodeId>>& reverse_node_map);

    Graph _graph;
};


#endif //MAXMATCHING_MAXIMUM_MATCHING_ALGORITHM_H
