#include <cassert>
#include "nested_shrunken_graph.h"

NestedShrunkenGraph::NestedShrunkenGraph(size_t num_nodes)
        : _current_partition(num_nodes) {}

Representative NestedShrunkenGraph::shrink(VertexSet const& to_shrink) {
    assert(to_shrink.size() > 1);
    auto old_partition = _current_partition;
    auto circuit_representative = _current_partition.find(to_shrink.at(0));
    for (size_t i = 1; i < to_shrink.size(); ++i) {
        auto const& root_to_add = _current_partition.find(to_shrink.at(i));
        circuit_representative = _current_partition.merge_roots(circuit_representative, root_to_add);
    }
    _shrink_stack.push_back({std::move(old_partition), to_shrink, circuit_representative});
    return circuit_representative;
}

Representative NestedShrunkenGraph::get_representative(NodeId const node) const {
    return _current_partition.find(node);
}

bool NestedShrunkenGraph::is_shrunken() const {
    return not _shrink_stack.empty();
}

std::pair<VertexSet, Representative> NestedShrunkenGraph::expand() {
    auto shrink_to_undo = std::move(_shrink_stack.back());
    _shrink_stack.pop_back();
    _current_partition = std::move(shrink_to_undo.old_partition);
    return {shrink_to_undo.shrunken_circuit, shrink_to_undo.shrunken_vertex};
}
