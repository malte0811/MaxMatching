#ifndef MAXMATCHING_ALTERNATING_TREE_H
#define MAXMATCHING_ALTERNATING_TREE_H

#include "matching.h"

class AlternatingTree {
public:
    AlternatingTree(Matching matching, NodeId root_node);

    std::vector<NodeId> shrink_fundamental_circuit(Representative end_a, Representative end_b);

    Matching&& augment_and_unshrink(Representative tree_node, Representative unmatched_neighbor);

    void extend(Representative tree_node, Representative matched_neighbor);

    Representative get_representative(NodeId node) const;

    bool is_tree_edge(Representative node_a, Representative node_b) const;

    bool is_tree_node(Representative node) const;

    bool is_even(Representative node) const;

private:
    struct ExtraShrinkData {
        // Potentially better structure: vector<Representative> mapping non-circuit vertex to circuit neighbor
        std::vector<std::vector<Representative>> circuit_neighbors_in_tree;
    };
    enum NodeStatus {
        not_in_tree,
        even,
        odd,
        root,
        //TODO remove or semi-disable using preprocessor commands
        not_representative,
    };

    NodeStatus get_state(Representative node) const;

    void set_state(Representative node, NodeStatus new_status);

    bool is_root(Representative node) const;

    void set_parent(Representative node, Representative parent);

    Representative get_parent(Representative node) const;

    Matching _current_matching;
    NestedShrinking _shrinking;
    std::vector<ExtraShrinkData> _shrink_stack;
    std::vector<NodeId> _parent_node;
    std::vector<std::vector<NodeId>> _children;
    std::vector<NodeStatus> _node_states;
};

#endif //MAXMATCHING_ALTERNATING_TREE_H
