#pragma once

#include "graph_basics.h"

#include <fstream>

namespace graph {
	void write_adjlist_to_edgelist_undirected(std::ofstream& out, const adjacency_list& G);
	adjacency_list read_adjlist_from_edgelist_undirected(std::ifstream& in, size_t skip_lines = 0);
}