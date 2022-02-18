#pragma once

#include "graph_basics.h"

namespace vertex_connectivity {
	class sparsification_labelling {
		struct edge {
			vertex x;
			vertex y;
			size_t k;
			bool operator<(const edge& other) const;
		};
		std::vector<edge> edges;
		vertex last_vertex = 0;
		size_t n = 0;
		bool sorted = false;

		sparsification_labelling();
	public:
		//sparsification_labelling(adjacency_list G); // Replaced by nagamochi_ibaraki_labelling
		adjacency_list get_sparsified_undirected_graph(size_t k);
		void sort();

		vertex get_last_vertex();
		size_t get_n();

		static sparsification_labelling nagamochi_ibaraki(const adjacency_list& G);
		static sparsification_labelling random(const adjacency_list& G, size_t k);
	};
}