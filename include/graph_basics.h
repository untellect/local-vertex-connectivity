#pragma once

#include <vector>
#include <unordered_set>

namespace vertex_connectivity {
	typedef unsigned long long int vertex;
	typedef std::vector<std::vector<vertex>> adjacency_list;
	typedef std::unordered_set<vertex> vertex_set;
	typedef std::vector<vertex> vertex_vec;
}
