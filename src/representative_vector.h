#ifndef MAXMATCHING_REPRESENTATIVE_VECTOR_H
#define MAXMATCHING_REPRESENTATIVE_VECTOR_H

#include <vector>
#include "representative.h"

/**
 * A wrapper around std::vector<ValueT> using Representative as the key. This should be completely removed by the
 * optimizer, but allows a clear separation between vectors indexed by nodes and vectors indexed by partition set
 * representatives
 */
template<typename ValueT>
class RepresentativeVector {
public:
    RepresentativeVector();

    explicit RepresentativeVector(size_t initial_size, ValueT const& value = ValueT());

    ValueT& at(Representative id);

    [[nodiscard]] ValueT const& at(Representative id) const;

    [[nodiscard]] size_t size() const;
private:
    std::vector<ValueT> _internal_vec;
};

template<typename ValueT>
inline RepresentativeVector<ValueT>::RepresentativeVector(): _internal_vec() {}

template<typename ValueT>
inline RepresentativeVector<ValueT>::RepresentativeVector(size_t initial_size, ValueT const& value):
_internal_vec(initial_size, value) {}

template<typename ValueT>
inline ValueT& RepresentativeVector<ValueT>::at(Representative id) {
    return _internal_vec.at(id.id());
}

template<typename ValueT>
inline ValueT const& RepresentativeVector<ValueT>::at(Representative id) const {
    return _internal_vec.at(id.id());
}

template<typename ValueT>
inline size_t RepresentativeVector<ValueT>::size() const {
    return _internal_vec.size();
}


#endif //MAXMATCHING_REPRESENTATIVE_VECTOR_H
