#include "util.h"

// So that graphutil can be linked without resettable_graph
namespace vertex_connectivity {
	vertex_set get_neighbors(const resettable_graph& G, const vertex_set& S) {
		return get_neighbors(G._adj, S);
	}

	vertex_set get_reachable(resettable_graph& G, vertex x) {
		G.visit(x);
		std::vector<vertex> Q = { x };
		while (!Q.empty()) {
			vertex v = Q.back(); Q.pop_back();
			for (vertex w : G[v]) {
				if (G.visited(w)) continue;
				G.visit(w);
				Q.push_back(w);
			}
		}

		vertex_set S(G._visited_vertices.begin(), G._visited_vertices.end());
		G.reset_changes();
		return S;
	}

	vertex_set get_reachable(resettable_graph& G, const vertex_vec& S) {	
		for(vertex x : S) {
			G.new_search();
			std::vector<vertex> Q = { x };
			while (!Q.empty()) {
				vertex v = Q.back(); Q.pop_back();
				for (vertex w : G[v]) {
					if (G.visited(w)) continue;
					G.visit(w);
					Q.push_back(w);
				}
			}
		}

		vertex_set reachable(G._visited_vertices.begin(), G._visited_vertices.end());
		G.reset_changes();
		return reachable;
	}

	edge_sampler::edge_sampler(const resettable_graph& G) {
		for (vertex s = 0; s < G.size(); s++) {
			for(vertex t : G[s]) {
				data.push_back({ s, t });
			}
		}
	}
}