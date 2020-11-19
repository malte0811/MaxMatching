#ifndef MAXMATCHING_REPRESENTATIVE_H
#define MAXMATCHING_REPRESENTATIVE_H

#include "graph.h"

/**
 * A representative of a set in a partition. This should be completely removed by compiler optimization.
 */
class Representative {
public:
    explicit Representative(NodeId id);

    Representative() : Representative(0) {}

    [[nodiscard]] NodeId id() const;

    bool operator==(Representative other) const;

    bool operator!=(Representative other) const;

private:
    NodeId _id;
};

using Representatives = std::vector<Representative>;

//Inline section

inline Representative::Representative(NodeId id) : _id(id) {}

inline NodeId Representative::id() const {
    return _id;
}

inline bool Representative::operator==(Representative other) const {
    return id() == other.id();
}

inline bool Representative::operator!=(Representative other) const {
    return not(*this == other);
}

#endif //MAXMATCHING_REPRESENTATIVE_H
