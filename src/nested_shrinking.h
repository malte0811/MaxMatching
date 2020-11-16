#ifndef MAXMATCHING_NESTED_SHRINKING_H
#define MAXMATCHING_NESTED_SHRINKING_H

#include "graph.h"
#include "representative_vector.h"

class NestedShrinking {
public:

    explicit NestedShrinking(size_t num_nodes);

    Representative shrink(RepresentativeSet const& to_shrink);

    /**
     * Undo one shrinking operation
     * @return The vertex set shrunken in the undone operation in the same order as it was passed to "shrink",
     * and the representative of the set after the shrinking/before the expansion
     */
    std::pair<RepresentativeSet, Representative> expand();

    [[nodiscard]] Representative get_representative(NodeId node) const;

    [[nodiscard]] bool is_shrunken() const;
private:
    struct SetReplacement {
        Representative old_name;
        std::vector<NodeId> affected_nodes;
    };
    struct ShrinkStep {
        Representative new_name;
        std::vector<SetReplacement> elements;
    };

    [[nodiscard]] size_t get_size(Representative const& set) const;

    [[nodiscard]] std::vector<NodeId> const& get_elements(Representative const& set) const;

    void validate() const;

    std::vector<Representative> _partition;
    RepresentativeVector<std::vector<NodeId>> _set_elements;

    std::vector<ShrinkStep> _shrink_stack;
};


#endif //MAXMATCHING_NESTED_SHRINKING_H
