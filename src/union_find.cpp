#include "union_find.hpp"

UnionFind::UnionFind(index_t size): entries(size) {
	for (index_t i = 0;i<size;++i) {
		entry& e = entries[i];
		e.rank = 0;
		e.parent = i;
	}
}

UnionFind::index_t UnionFind::find(index_t start) const {
	auto& curr_entry = entries[start];
	if (curr_entry.parent == start) {
		return start;
	}
	auto& parent = entries[curr_entry.parent];
	if (parent.parent == curr_entry.parent) {
		return curr_entry.parent;
	}
	auto const& root = find(parent.parent);
    curr_entry.parent = root;
	parent.parent = root;
	return root;
}

UnionFind::index_t UnionFind::merge(index_t start_a, index_t start_b) {
	return merge_roots(find(start_a), find(start_b));
}

UnionFind::index_t UnionFind::merge_roots(index_t root_a, index_t root_b) {
	if (root_a == root_b) {
		return root_a;
	}
	auto& entry_a = entries[root_a];
	auto& entry_b = entries[root_b];
	if (entry_a.rank > entry_b.rank) {
        entry_b.parent = root_a;
		return root_a;
	} else {
        entry_a.parent = root_b;
		if (entry_a.rank == entry_b.rank) {
			++entry_a.rank;
		}
		return root_b;
	}
}
