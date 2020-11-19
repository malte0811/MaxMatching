#ifndef MAXMATCHING_MAXIMUM_MATCHING_ALGORITHM_H
#define MAXMATCHING_MAXIMUM_MATCHING_ALGORITHM_H


#include "graph.h"
#include "matching.h"

class MaximumMatchingAlgorithm {
public:
    explicit MaximumMatchingAlgorithm(Graph const& graph);

    EdgeList calc_maximum_matching();

private:
    void match_leaves();

    Graph const& _graph;
    Matching _current_matching;
    std::vector<char> _allowed;
    size_t _num_blocked_nodes = 0;
};


#endif //MAXMATCHING_MAXIMUM_MATCHING_ALGORITHM_H
