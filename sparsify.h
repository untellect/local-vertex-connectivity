#pragma once

#include "graph_basics.h"

namespace graph {
	class sparsification_labelling {
		struct edge {
			vertex x;
			vertex y;
			size_t k;
			bool operator<(const edge& other) const {
				return k < other.k;
			}
		};
		std::vector<edge> edges;
		vertex last_vertex = 0;
		size_t n = 0;
		bool sorted = false;

		sparsification_labelling() {}
	public:
		//sparsification_labelling(adjacency_list G); // Replaced by nagamochi_ibaraki_labelling
		adjacency_list get_sparsified_undirected_graph(size_t k);
		void sort();

		vertex get_last_vertex();
		size_t get_n();

		friend sparsification_labelling nagamochi_ibaraki_labelling(const adjacency_list& G);
		friend sparsification_labelling random_labelling(const adjacency_list& G, size_t k);
	};
}