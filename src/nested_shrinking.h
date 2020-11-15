#ifndef MAXMATCHING_NESTED_SHRINKING_H
#define MAXMATCHING_NESTED_SHRINKING_H

#include "graph.h"

class Representative {
public:
    explicit Representative(NodeId id);

    Representative(): Representative(0) {}

    [[nodiscard]] NodeId id() const;

    bool operator==(Representative other) const;

    bool operator!=(Representative other) const;
private:
    NodeId _id;
};
using VertexSet = std::vector<Representative>;

class NestedShrinking {
public:

    explicit NestedShrinking(size_t num_nodes);

    Representative shrink(VertexSet const& to_shrink);

    /**
     * Undo one shrinking operation
     * @return The vertex set shrunken in the undone operation and the representative of the set after the
     * shrinking/before the expansion
     */
    std::pair<VertexSet, Representative> expand();

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

    [[nodiscard]] std::vector<NodeId>& get_elements(Representative const& set);

    void validate() const;

    std::vector<Representative> _partition;
    std::vector<std::vector<NodeId>> _set_elements;

    std::vector<ShrinkStep> _shrink_stack;
};

//Inline section

inline Representative::Representative(NodeId id) : _id(id) {}

inline NodeId Representative::id() const {
    return _id;
}

inline bool Representative::operator==(Representative other) const {
    return id() == other.id();
}

inline bool Representative::operator!=(Representative other) const {
    return not (*this == other);
}


#endif //MAXMATCHING_NESTED_SHRINKING_H
