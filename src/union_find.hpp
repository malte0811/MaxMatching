#ifndef UNION_FIND_HPP
#define UNION_FIND_HPP

#include <vector>
#include <cstdint>

class UnionFind {
public:
	using index_t = uint32_t;
	using rank_t = uint32_t;
	explicit UnionFind(index_t size);
	index_t find(index_t start) const;
	index_t merge(index_t start_a, index_t start_b);
	// Similar to merge, but assumes that a and b are roots/representatives already
	index_t merge_roots(index_t root_a, index_t root_b);
private:
	struct entry {
		index_t parent;
		rank_t rank;
	};
	//mutable: find is "logically" const, but technically mutates this vector
	std::vector<entry> mutable entries;
};
#endif
