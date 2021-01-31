#include "pch.h"

#include "debug.h"

namespace debug {
	std::vector<localec_debug> local_debug, localdeg_debug, localhdeg_debug;
	std::string current_debug_id = "";

	std::string localec_debug::get_id() { return id; }
	size_t localec_debug::get_n() { return n; }
	size_t localec_debug::get_v() { return v; }
	size_t localec_debug::get_vertices_visited() { return vertices_visited; }
	size_t localec_debug::get_edges_visited() { return edges_visited; }

	void localec_debug::finalise() {
		if (finalised) return;

		sort(all_visited_vertices.begin(), all_visited_vertices.end());
		all_visited_vertices.erase(unique(all_visited_vertices.begin(), all_visited_vertices.end()), all_visited_vertices.end());
		unique_vertices_visited = all_visited_vertices.size();
		all_visited_vertices.clear();
		all_visited_vertices.shrink_to_fit();

		sort(all_visited_edges.begin(), all_visited_edges.end());
		all_visited_edges.erase(unique(all_visited_edges.begin(), all_visited_edges.end()), all_visited_edges.end());
		unique_edges_visited = all_visited_edges.size();
		all_visited_edges.clear();
		all_visited_edges.shrink_to_fit();

		finalised = true;
	}
	size_t localec_debug::get_unique_vertices() {
		finalise();
		return unique_vertices_visited;
	}
	size_t localec_debug::get_unique_edges() {
		finalise();
		return unique_edges_visited;
	}

	void localec_debug::visit_vertex(graph::vertex x) {
		if (finalised) throw 0;

		vertices_visited++;
		all_visited_vertices.push_back(x);
	}
	void localec_debug::visit_edge(graph::vertex x, graph::vertex y) {
		if (finalised) throw 0;

		edges_visited++;
		all_visited_edges.push_back({ x, y });
	}
}