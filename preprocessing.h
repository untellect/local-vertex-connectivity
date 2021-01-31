#pragma once

#include "graph_basics.h"

namespace preprocessing {
	using namespace graph;

	adjacency_list get_subgraph(const adjacency_list& G, const std::vector<vertex>& V);
	adjacency_list kcore_undirected(const adjacency_list& G, size_t k);
	adjacency_list largest_connected_component_undirected(const adjacency_list& G);
	void inplace_remove_duplicate_edges(adjacency_list& G);
}