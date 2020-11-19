#ifndef MAXMATCHING_ALTERNATING_TREE_H
#define MAXMATCHING_ALTERNATING_TREE_H

#include "matching.h"
#include "representative_vector.h"

class AlternatingTree {
public:
    AlternatingTree(Matching& matching, NodeId root_node);

    [[nodiscard]] std::vector<NodeId> shrink_fundamental_circuit(
            Representative repr_a, Representative repr_b, NodeId node_a, NodeId node_b
    );

    void augment_and_unshrink(Representative tree_repr, NodeId tree_node, Representative neighbor);

    void unshrink();

    void extend(Representative tree_repr, Representative matched_repr, NodeId tree_node);

    [[nodiscard]] Representative get_representative(NodeId node) const;

    [[nodiscard]] bool is_tree_node(Representative node) const;

    [[nodiscard]] bool is_even(Representative node) const;

    void reset(NodeId root_node);

    [[nodiscard]] std::vector<NodeId> get_tree_vertices() const;

private:
    static auto constexpr invalid_node = std::numeric_limits<NodeId>::max();

    struct Parent {
        // Nodes within the pseudonode on the edge used to connect the pseudonodes
        NodeId edge_end_here;
        NodeId edge_end_parent;
    };

    struct FundamentalCircuit {
        struct NodeInfo {
            // pseudonode
            Representative repr;
            // Nodes within the pseudonodes for the edges above (closer to the root) and below this node
            NodeId below;
            NodeId above;
        };
        std::vector<NodeInfo> path_containing_top_node;
        std::vector<NodeInfo> path_without_top_node;
        //Node in the top pseudonode used to attach the path without the top node
        NodeId other_vertex_used_at_top;

        [[nodiscard]] std::pair<EdgeList, RepresentativeSet> to_edges_and_reprs() const;
    };

    enum NodeStatus {
        not_in_tree,
        even,
        odd,
        root,
        not_representative,
    };

    [[nodiscard]] NodeStatus get_state(Representative node) const;

    void set_state(Representative node, NodeStatus new_status);

    [[nodiscard]] FundamentalCircuit find_fundamental_circuit(
            Representative repr_a, Representative repr_b, NodeId node_a, NodeId node_b
    ) const;

    [[nodiscard]] bool is_root(Representative node) const;

    void set_parent(Representative node, Representative parent_rep, NodeId parent, NodeId link);

    [[nodiscard]] Representative get_parent_repr(Representative node) const;

    [[nodiscard]] std::pair<NodeId, NodeId> get_edge_to_parent(Representative node) const;

    [[nodiscard]] NodeId get_depth(Representative node) const;

    Matching& _current_matching;
    NestedShrinking _shrinking;
    RepresentativeVector<Parent> _parent_node;
    RepresentativeVector<NodeId> _depth;
    std::vector<NodeStatus> _node_states;
    std::vector<NodeId> _tree_vertices;
    /// Indicates whether this tree is still in a valid state or needs to be reset before any further operations
    /// (this is the case after unshrinking)
    bool _needs_reset = true;
};

#endif //MAXMATCHING_ALTERNATING_TREE_H
