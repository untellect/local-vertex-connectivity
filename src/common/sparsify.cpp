#include "sparsify.h"
#include "random.h"
#include "util.h"

#include <queue>

namespace vertex_connectivity {
	bool sparsification_labelling::edge::operator<(const edge& other) const {
		return k < other.k;
	}

	sparsification_labelling::sparsification_labelling() {};

	sparsification_labelling sparsification_labelling::nagamochi_ibaraki(const adjacency_list& G) {
		sparsification_labelling labelling;
		size_t n = labelling.n = G.size();

		std::vector<bool> visited(n, false);
		std::vector<size_t> r(n, 0);
		struct pq_element {
			size_t r;
			vertex x;
			bool operator<(const pq_element& other) const {
				return r < other.r;
			}
		};
		std::priority_queue<pq_element> pq;
		for (vertex x = 0; x < n; x++) pq.push({ 0, x });

		vertex x = 0;
		size_t r_x;
		while (!pq.empty()) {
			x = pq.top().x;
			r_x = pq.top().r;
			pq.pop();
			if (r[x] != r_x) continue; // ignore duplicates in the priority queue
			if (visited[x]) continue; // Only interested in unvisited x.
			visited[x] = true;
			for (vertex y : G[x]) {
				if (visited[y]) continue; // Only interested in unvisited y.
				labelling.edges.push_back({ x, y, r[y] + 1 });
				r[y] += 1;
				pq.push({ r[y], y });
			}
		}
		labelling.last_vertex = x;

		return labelling;
	}

	sparsification_labelling sparsification_labelling::random(const adjacency_list& G, size_t k) {
		sparsification_labelling labelling;
		size_t n = labelling.n = G.size();

		struct edge { vertex x, y; };
		std::vector<edge> edges;
		for (vertex x = 0; x < n; x++) {
			for (const vertex& y : G[x]) {
				edges.push_back({ x, y });
			}
		}
		util::random_shuffle(edges);
		
		util::multi_union_find forest_sets(n, k);
		for (edge& edge : edges) {
			size_t x = edge.x;
			size_t y = edge.y;
			// i is the lowest possible such that x and y are not in the same tree in F_i.
			size_t i = forest_sets.lowest_not_same(x, y);
			if (i < k) {
				labelling.edges.push_back({ x, y, i });
				forest_sets.merge(x, y, i);
			}
		}

		return labelling;
	}

	adjacency_list sparsification_labelling::get_sparsified_undirected_graph(size_t k) {
		adjacency_list G(n);
		for (auto& e : edges) {
			if (e.k > k) {
				if (sorted)
					return G; // The rest have e.k > k.
				else
					continue;
			}

			G[e.x].push_back(e.y);
			G[e.y].push_back(e.x);
		}
		return G;
	}

	vertex sparsification_labelling::get_last_vertex() {
		return last_vertex;
	}

	size_t sparsification_labelling::get_n() {
		return n;
	}

	void sparsification_labelling::sort() {
		if (sorted)
			return;
		std::sort(edges.begin(), edges.end());
		sorted = true;
	}
}