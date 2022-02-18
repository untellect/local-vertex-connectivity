#pragma once

#include "graph_basics.h"

#include <fstream>

namespace vertex_connectivity {
	void write_adjlist_to_edgelist_undirected(std::ostream& out, const adjacency_list& G);
	adjacency_list read_adjlist_from_edgelist_undirected(std::istream& in);

	void write_adjlist_to_edgelist_directed(std::ostream& out, const adjacency_list& G);
	adjacency_list read_adjlist_from_edgelist_directed(std::istream& in);
}