#ifndef MAXMATCHING_MAXIMUM_MATCHING_ALGORITHM_H
#define MAXMATCHING_MAXIMUM_MATCHING_ALGORITHM_H


#include "graph.hpp"

class MaximumMatchingAlgorithm {
public:
    explicit MaximumMatchingAlgorithm(Graph&& graph);

    EdgeSet calc_maximum_matching();
private:
    Graph _graph;
};


#endif //MAXMATCHING_MAXIMUM_MATCHING_ALGORITHM_H
