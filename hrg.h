#pragma once

#include "vertex_connectivity.h"

#include <deque>

namespace vertex_connectivity {

	// Encapsulates the main part of the algorithm by Henzinger, Rao and Gabow (2000)
	class hrg
	{
		size_t n;
		adjacency_list SG; // Residual split graph.
		std::vector<size_t> current_edge_i;

		std::vector<bool> in_W;
		std::deque<vertex> W_and_overflowing; // Vertices here are guaranteed to be in W but not different from the sink t.

		std::deque<std::vector<vertex>> W_by_distance;
		std::vector<size_t> wbd_i; // (new auxiliary data structure) W_by_distance[d[v] - d_min_W][wbd_i[v]] = v  
		size_t d_min_W = 0;

		std::vector<vertex> best_W;
		std::vector<vertex> best_W_neighbors;
		size_t best_W_indegree = SIZE_MAX;

		std::vector<size_t> d;
		std::vector<size_t> excess;

		std::vector<std::vector<vertex>> D = { std::vector<vertex>() };
		size_t gamma = 0;
		std::vector<size_t> d_min_D_and_T = { SIZE_MAX };
		std::vector<size_t> d_min_D = { SIZE_MAX }; // These help with merging D[gamma] into W.
		std::vector<size_t> d_max_D = { 0 };

		vertex target;

		std::vector<vertex>& S() { return D[0]; }; // D[0] is the source set.
		inline bool in_T(size_t v) { return (v & 1) == 0; } // v is even/invertex

		void new_sink();
		void push_from(size_t v);
		void relabel(size_t v);
		void create_dormant(size_t v);
		void create_singleton_dormant_or_join(size_t v);

		/* procedure initialize (plus constructing the split graph) */
		hrg(const adjacency_list& G, vertex s);
	public:
		typedef void (min_vc_split_fn)(const adjacency_list& G, size_t source, size_t k, vertex_set& cut, bool& found_cut);
		static min_vc_split_fn min_vc_split_v1, min_vc_split_v2, min_vc_split_v3;
	};
}
