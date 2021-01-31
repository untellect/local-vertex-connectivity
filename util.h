#pragma once

#include "graph_basics.h"
#include "resettable_graph.h"

#include <vector>
#include <numeric>

namespace util {
	class union_find {
		std::vector<size_t> parent;
		std::vector<size_t> rank;
		size_t distinct_sets;
	public:
		union_find(size_t n);
		size_t find(size_t x);
		void merge(size_t x, size_t y);
		size_t count_distinct_sets() const;
	};

	/*
	 * This class encapsulates k union_find data structures and can be used to find the lowest i
	 * such that two elements are not in the same set in union_find data structure number i.
	 */
	class multi_union_find {
		std::vector<union_find> data;
		size_t lowest_nontrivial = 0;
	public:
		const size_t n, k;
		multi_union_find(size_t n, size_t k);
		size_t lowest_not_same(size_t x, size_t y);
		void merge(size_t x, size_t y, size_t k);
		void trivialise(size_t i);
		bool is_nontrivial() const;
	};
}

namespace graph {
	vertex_set get_reachable(const resettable_graph& G, vertex x);
	vertex_set get_reachable(const adjacency_list& G, vertex x);
	vertex_set get_neighbors(const adjacency_list& G, const vertex_set& S);
	vertex_set get_neighbors(const resettable_graph& G, const vertex_set& S);
	vertex_set get_unsplit(const vertex_set& S_split);

	/*
	 * In order to reverse an edge e = (x_out->y_in) in the split graph, one of the following must be true:
	 * - x is the source vertex
	 * - (x_in, x_out) is also reversed
	 * In the former case, e may be a cut edge. In the latter, it may not. Therefore the set of cut edges is:
	 * {(x_in, x_out) : x_in is in L, x_out is not in L} +
	 * {(source_out, x_in) : x_in adjacent to source_out is not in L}
	 * where L is the set of reachable vertices
	 */
	vertex_set get_unsplit_cut_from_reachable_set(const vertex_set& L_split, const std::vector<vertex>& adjacent_to_source_unsplit);

	bool is_valid_vertex_cut(const adjacency_list& G, const vertex_set& S);

	// Samples an edge and provides the source vertex
	struct edge_sampler {
		struct edge { vertex x, y; };
		std::vector<edge> data;
		size_t i = 0;

		edge_sampler(const resettable_graph& G);
		edge_sampler(const adjacency_list& G);
		vertex get_source();
		vertex get_target();
		size_t count_edges();
		void reset();
	};
}
