#include "pch.h"

#include "preprocessing.h"
#include "util.h"

namespace preprocessing {
	using namespace graph;

	adjacency_list get_subgraph(const adjacency_list& G, const std::vector<vertex>& V) {
		size_t n = G.size(); // old n
		size_t n_v = V.size(); // new n;

		std::vector<size_t> old_to_new_mapping(n, SIZE_MAX);
		for (size_t i = 0; i < n_v; i++) {
			old_to_new_mapping[V[i]] = i;
		}

		adjacency_list G_V(n_v);
		// For each vertex v, copy over the old adjacency list, translating them to the new mapping and skipping any removed vertices.
		for (size_t v = 0; v < n_v; v++) {
			size_t v_old = V[v];
			for (size_t w_old : G[v_old]) {
				size_t w = old_to_new_mapping[w_old];
				if (w == SIZE_MAX) continue;
				G_V[v].push_back(w); // Other direction will be added when we iterate over G[w]
			}
		}

		return G_V;
	}

	adjacency_list kcore_undirected(const adjacency_list& G, size_t k) {
		size_t n = G.size();
		std::vector<size_t> degree(n);
		std::vector<vertex> low_degree;
		for (vertex v = 0; v < n; v++) {
			degree[v] = G[v].size();
			if (G[v].size() < k)
				low_degree.push_back(v);
		}
		while (!low_degree.empty()) {
			vertex v = low_degree.back();
			low_degree.pop_back();
			for (vertex w : G[v]) {
				if (degree[w] < k) continue; // Already low degree.
				if ((--degree[w]) < k)
					low_degree.push_back(w);
			}
		}
		std::vector<vertex> kcore_vertices;
		for (vertex v = 0; v < n; v++) {
			if (degree[v] >= k) {
				kcore_vertices.push_back(v);
			}
		}
		return get_subgraph(G, kcore_vertices);
	}
	adjacency_list largest_connected_component_undirected(const adjacency_list& G) {
		size_t n = G.size();
		std::vector<std::vector<size_t>> connected_components;
		{
			std::vector<bool> visited(n, false);
			for (size_t starting_vertex = 0; starting_vertex < n; starting_vertex++) {
				if (visited[starting_vertex]) continue;

				std::vector<size_t> component_vertices;
				std::vector<size_t> Q = { starting_vertex };
				while (!Q.empty()) {
					size_t v = Q.back();
					Q.pop_back();

					if (visited[v]) continue;
					visited[v] = true;
					component_vertices.push_back(v);

					for (size_t w : G[v])
						Q.push_back(w);
				}

				connected_components.push_back(component_vertices);
			}
		}
		size_t i_LCC = 0;
		for (size_t i = 0; i < connected_components.size(); i++) {
			if (connected_components[i_LCC].size() < connected_components[i].size())
				i_LCC = i;
		}
		std::vector<size_t>& LCC_vertices = connected_components[i_LCC];

		return get_subgraph(G, LCC_vertices);
	}

	void inplace_remove_duplicate_edges(adjacency_list& G) {
		for (auto& vec : G) {
			sort(vec.begin(), vec.end());
			auto& it = unique(vec.begin(), vec.end());
			vec.erase(it, vec.end());
		}
	}
}