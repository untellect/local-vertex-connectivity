#pragma once

#include <vector>
#include <unordered_set>

namespace graph {
	typedef size_t vertex;
	typedef std::vector<std::vector<vertex>> adjacency_list;
	typedef std::unordered_set<vertex> vertex_set;
}
