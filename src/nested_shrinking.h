#ifndef MAXMATCHING_NESTED_SHRINKING_H
#define MAXMATCHING_NESTED_SHRINKING_H

#include "graph.h"
#include "representative_vector.h"

/**
 * Stores a partition obtained by successive merging of sets in the partition, in addition to providing a way of
 * reverting these merging steps
 */
class NestedShrinking {
public:

    explicit NestedShrinking(size_t num_nodes);

    /**
     * Perform one shrinking operation
     * @param to_shrink The vertices to shrink
     * @return the representative of the merged set
     */
    Representative shrink(Representatives const& to_shrink);

    /**
     * Undo one shrinking operation
     * @return The vertex set shrunken in the undone operation in the same order as it was passed to "shrink",
     * and the representative of the set after the shrinking/before the expansion
     */
    std::pair<Representatives, Representative> expand();

    [[nodiscard]] Representative get_representative(NodeId node) const;

    [[nodiscard]] bool is_shrunken() const;

private:
    struct SetReplacement {
        /// "name" of the set before the shrinking
        Representative old_name;
        /// Members of the set before the shrinking
        std::vector<NodeId> affected_nodes;
    };
    struct ShrinkStep {
        /// "name" of the union of the relevant sets after shrinking
        Representative new_name;
        /// The sets that were combined. If the old_name of a set is new_name, the set of affected_nodes is empty
        /// (This makes restoring the order of representatives used in the shrinking trivial)
        std::vector<SetReplacement> elements;
    };

    [[nodiscard]] size_t get_size(Representative const& set) const;

    [[nodiscard]] std::vector<NodeId> const& get_elements(Representative const& set) const;

    void validate() const;

    /// Maps a vertex to its representative
    std::vector<Representative> _partition;
    /// Maps a representative to the vertices in the corresponding set
    RepresentativeVector<std::vector<NodeId>> _set_elements;

    std::vector<ShrinkStep> _shrink_stack;
};


#endif //MAXMATCHING_NESTED_SHRINKING_H
