#pragma once

#include "config.h"
#include "graph_basics.h"


#include <fstream>
#include <unordered_set>

#include <set>
#include <utility>

namespace debug {
	constexpr bool localec_on = false; // Can turn off all localec debugging.
	constexpr bool hrg_decision = false;
	constexpr bool any_debug = localec_on || hrg_decision;

	extern std::string current_debug_id;

	class localec_debug {
	private:
		std::string id;
		size_t n, v, vertices_visited = 0, edges_visited = 0, unique_vertices_visited = 0, unique_edges_visited = 0;
		std::vector<graph::vertex> all_visited_vertices;
		std::vector<std::pair<graph::vertex, graph::vertex>> all_visited_edges;
		bool finalised = false; // We save vertices/edges with repeated values, then sort+unique to find the number of distinct vertices/edges
	public:

		std::string get_id();
		size_t get_n();
		size_t get_v();
		size_t get_vertices_visited();
		size_t get_edges_visited();
		size_t get_unique_vertices();
		size_t get_unique_edges();

		void finalise();
		void visit_vertex(graph::vertex x);
		void visit_edge(graph::vertex x, graph::vertex y);
		localec_debug(size_t n, size_t v) : n(n), v(v) {
			id = current_debug_id;
		}
	};

	extern std::vector<localec_debug> local_debug, localdeg_debug, localhdeg_debug;
}