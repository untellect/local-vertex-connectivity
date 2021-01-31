#pragma once

#include "resettable_graph.h"

namespace maxflow {
	using namespace graph;

	size_t ford_fulkerson_dfs(adjacency_list& SG, vertex x_out, vertex y_in, size_t k);
	size_t ford_fulkerson_dfs(resettable_graph& SG, vertex x_out, vertex y_in, size_t k);
}