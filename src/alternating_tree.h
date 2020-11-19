#ifndef MAXMATCHING_ALTERNATING_TREE_H
#define MAXMATCHING_ALTERNATING_TREE_H

#include "matching.h"
#include "representative_vector.h"

class AlternatingTree {
public:
    AlternatingTree(Matching& matching, NodeId root_node);

    /**
     * Shrink a fundamental circuit in the tree.
     * @param repr_a Pseudonode on one end of the edge inducing the fundamental cycle
     * @param node_a Real node on that end of the edge
     * @param repr_b similar to repr_a
     * @param node_b similar to node_a
     * @return Odd nodes in the circuit (these were not pseudonodes before, so returning node IDs correct)
     */
    [[nodiscard]] std::vector<NodeId> shrink_fundamental_circuit(
            Representative repr_a, NodeId node_a, Representative repr_b, NodeId node_b
    );

    /**
     * Perform an augmentation step and call AlternatingTree::unshrink. The warning on unshrink applies.
     * @param tree_repr Pseudonode of the tree the edge to an unmatched node is attached to
     * @param tree_node The end node of the pseudonode inside the tree
     * @param neighbor The unmatched neighbor. The node is not in the tree, so the representative is the same as the
     * node ID
     */
    void augment_and_unshrink(Representative tree_repr, NodeId tree_node, NodeId neighbor);

    /**
     * Fully unshrink the matching and the nested shrinking stored in this tree
     * Warning: After this operation, only get_tree_vertices may be called before reset is called
     */
    void unshrink();

    /**
     * Extend the tree using the given edge to a matched vertex outside the tree
     * @param tree_repr The pseudonode the edge is attached to
     * @param tree_node The node within the pseudonode the edge is attached to
     * @param matched_node The matched vertex at the other end of the edge. This is not a tree node, so the
     * node ID is the representative
     */
    void extend(Representative tree_repr, NodeId tree_node, NodeId matched_node);

    [[nodiscard]] Representative get_representative(NodeId node) const;

    [[nodiscard]] bool is_tree_node(Representative node) const;

    [[nodiscard]] bool is_even(Representative node) const;

    void reset(NodeId root_node);

    [[nodiscard]] std::vector<NodeId> get_tree_vertices() const;

private:
    static auto constexpr invalid_node = std::numeric_limits<NodeId>::max();

    struct EdgeToParent {
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

        /**
         * Convert the cycle to edges and vertex representatives
         * @return a pair of
         * - a vector of the vertex representatives in the cycle, starting at one of the endpoints of the edge that
         * generated this fundamental cycle
         * - a vector of the edges in the cycle, fulfilling the requirements for Matching::shrink when used together
         * with the first vector
         */
        [[nodiscard]] std::pair<Representatives, EdgeList> to_edges_and_reprs() const;
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

    void set_parent(NodeId non_tree_node, Representative parent_rep, NodeId parent);

    [[nodiscard]] Representative get_parent_repr(Representative node) const;

    [[nodiscard]] std::pair<NodeId, NodeId> get_edge_to_parent(Representative node) const;

    [[nodiscard]] NodeId get_depth(Representative node) const;

    Matching& _current_matching;
    NestedShrinking _shrinking;
    RepresentativeVector<EdgeToParent> _parent_edges;
    /// This is not actually the depth once shrinkings are performed, but it is still strictly monotonous along any
    /// path from the root, which is enough for the fast algorithm for finding fundamental cycles
    RepresentativeVector<NodeId> _depth;
    std::vector<NodeStatus> _node_states;
    std::vector<NodeId> _tree_vertices;
    /// Indicates whether this tree is still in a valid state or needs to be reset before any further operations
    /// (this is the case after unshrinking)
    bool _needs_reset = true;
};

#endif //MAXMATCHING_ALTERNATING_TREE_H
