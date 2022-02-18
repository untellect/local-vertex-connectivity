#include "util.h"
#include "random.h"

namespace util {
    union_find::union_find(size_t n) : parent(n), rank(n, 0), distinct_sets(n) {
        std::iota(parent.begin(), parent.end(), 0);
    }

	// Using "path halving"
	size_t union_find::find(size_t x) {
		while (parent[x] != x) {
			parent[x] = parent[parent[x]];
			x = parent[x];
		}
		return x;
	}

	// Using "rank"
	void union_find::merge(size_t x, size_t y) {
		x = find(x);
		y = find(y);

		if (x == y) return;

        distinct_sets--;

        if (rank[x] < rank[y])
            std::swap(x, y);
        
        parent[y] = x;
		
		if (rank[x] == rank[y])
			rank[x]++;
	}

    size_t union_find::count_distinct_sets() const {
        return distinct_sets;
    }

    multi_union_find::multi_union_find(size_t n, size_t k) : n(n), k(k) {
        for (size_t i = 0; i < k; i++) {
            data.emplace_back(n);
        }
    }

	size_t multi_union_find::lowest_not_same(size_t x, size_t y) {
		size_t low = lowest_nontrivial;
		size_t high = k; // maximum + 1
		while (low != high) {
			size_t mid = (high + low) / 2;
			bool same = data[mid].find(x) == data[mid].find(y);
			if (same) {
				low = mid + 1;
			}
			else {
				high = mid;
			}
		}
		return low;
	}

	void multi_union_find::merge(size_t x, size_t y, size_t k) {
		data[k].merge(x, y);
        if (data[k].count_distinct_sets() == 1)
            trivialise(k);
	}

    void multi_union_find::trivialise(size_t i) {
        lowest_nontrivial = std::max(lowest_nontrivial, i + 1);
    }

    bool multi_union_find::is_nontrivial() const {
        return lowest_nontrivial != k;
    }
}
