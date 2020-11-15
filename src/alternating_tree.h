#ifndef MAXMATCHING_ALTERNATING_TREE_H
#define MAXMATCHING_ALTERNATING_TREE_H

#include "matching.h"

class AlternatingTree {
public:
    AlternatingTree(Matching& matching, NodeId root_node);

    std::vector<NodeId> shrink_fundamental_circuit(Representative repr_a, Representative repr_b, NodeId node_a, NodeId node_b);

    void augment_and_unshrink(Representative tree_repr, NodeId tree_node, Representative neighbor);

    void unshrink();

    void extend(Representative tree_repr, Representative matched_repr, NodeId tree_node);

    Representative get_representative(NodeId node) const;

    bool is_tree_node(Representative node) const;

    bool is_even(Representative node) const;

    void reset(NodeId root_node);

    [[nodiscard]] std::vector<NodeId> get_tree_vertices() const;
private:
    struct Parent {
        NodeId edge_end_here;
        NodeId edge_end_parent;
    };
    struct ExtraShrinkData {
        Representative top_node;
        Parent old_shrink_parent;
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

    void set_parent(Representative node, Representative parent_rep, NodeId parent, NodeId link);

    Representative get_parent_repr(Representative node) const;

    std::pair<NodeId, NodeId> get_edge_to_parent(Representative node) const;

    Matching& _current_matching;
    NestedShrinking _shrinking;
    std::vector<ExtraShrinkData> _shrink_stack;
    std::vector<Parent> _parent_node;
    std::vector<NodeStatus> _node_states;
    std::vector<NodeId> _tree_vertices;
};

#endif //MAXMATCHING_ALTERNATING_TREE_H
