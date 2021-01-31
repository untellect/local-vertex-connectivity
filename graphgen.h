#pragma once

#include "graph_basics.h"

namespace graph {
	/*
	 * vertex i is split into invertex (2 * i) and outvertex (2 * i + 1).
	 */
	adjacency_list split_graph(const adjacency_list& G);
	adjacency_list random_forest_graph_with_planted_cut(size_t L, size_t S, size_t R, size_t k);
	adjacency_list pathological_forest_graph_with_planted_cut(size_t L, size_t S, size_t R, size_t k);
	// For debugging purposes.
	struct LSR_and_sets {
		adjacency_list G;
		vertex_set L;
		vertex_set S;
		vertex_set R;
	};
	LSR_and_sets random_forest_graph_with_planted_cut_and_sets(size_t L, size_t S, size_t R, size_t k);
	adjacency_list random_forest_graph_with_planted_cut_v2(size_t L, size_t S, size_t R, size_t k);


}