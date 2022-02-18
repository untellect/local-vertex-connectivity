#pragma once

#include <vector>

namespace vertex_connectivity::debug {
	struct localec_call {
		size_t nu = 0;
		bool found_cut = false;
		size_t edges_visited = 0;
		size_t vertices_visited = 0;
		size_t unique_vertices_visited = 0;
	};
	extern std::vector<localec_call> localec_log;
}