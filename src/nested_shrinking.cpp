#include <cassert>
#include "nested_shrinking.h"

NestedShrinking::NestedShrinking(size_t num_nodes)
        : _current_partition(num_nodes) {}

Representative NestedShrinking::shrink(VertexSet const& to_shrink) {
    assert(to_shrink.size() > 1);
    auto old_partition = _current_partition;
    auto circuit_representative = to_shrink.at(0);
    for (size_t i = 1; i < to_shrink.size(); ++i) {
        auto const& merged_id = _current_partition.merge_roots(circuit_representative.id(), to_shrink.at(i).id());
        circuit_representative = Representative(merged_id);
    }
    _shrink_stack.push_back({std::move(old_partition), to_shrink, circuit_representative});
    return circuit_representative;
}

Representative NestedShrinking::get_representative(NodeId const node) const {
    return Representative(_current_partition.find(node));
}

bool NestedShrinking::is_shrunken() const {
    return not _shrink_stack.empty();
}

std::pair<VertexSet, Representative> NestedShrinking::expand() {
    auto shrink_to_undo = std::move(_shrink_stack.back());
    _shrink_stack.pop_back();
    _current_partition = std::move(shrink_to_undo.old_partition);
    return {shrink_to_undo.shrunken_set, shrink_to_undo.resulting_vertex};
}
