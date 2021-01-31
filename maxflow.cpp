#include "pch.h"

#include "maxflow.h"

namespace maxflow {

	size_t ford_fulkerson_dfs(adjacency_list& SG, vertex source, vertex target, size_t k) {
		std::vector<vertex> prev(SG.size());
		prev[source] = source;
		std::vector<size_t> prev_i(SG.size());
		std::vector<bool> visited(SG.size());
		std::vector<vertex> Q;

		size_t flow = 0;

		// Start the DFS
		std::fill(visited.begin(), visited.end(), false);
		visited[source] = true;
		Q.clear();
		Q.push_back(source);

		while (!Q.empty()) {
			vertex v = Q.back(); Q.pop_back();
			for (size_t i = 0; i < SG[v].size(); i++) {
				vertex w = SG[v][i];
				if (visited[w])
					continue;

				visited[w] = true;
				prev[w] = v;
				prev_i[w] = i;
				Q.push_back(w);

				// Reverse the path.
				if (w == target) {
					flow++;

					// RG[w].push_back(v); // Add (t -> v) // This is actually pointless because it's an outgoing edge from t.

					vertex p = prev[v];
					while (p != v) {
						SG[v][i] = p; // Replace (v -> w) with (v -> p)
						//w = v;
						i = prev_i[v];
						v = p;
						p = prev[v];
					}
					SG[v][i] = SG[v].back(); // Remove (s -> w)
					SG[v].pop_back();

					// Restart the DFS
					std::fill(visited.begin(), visited.end(), false);
					visited[source] = true;
					Q.clear();
					Q.push_back(source);
					break;
				}
			}
		}

		return flow;
	}

	size_t ford_fulkerson_dfs(resettable_graph& SG, vertex source, vertex target, size_t k) {
		size_t flow = 0;

		SG.new_search();
		SG.visit(source, { source, 0 });
		resettable_graph::internal_location dfs = { source, 0 }; // dfs "current" pointer

		if (k == 0) return 0; // Trivial case.
		while (true) {
			if (dfs.v == target) {
				flow++;
				SG.reverse_source_to_vertex(target);
				if (flow >= k) return flow; // At most k iterations.

				// Reset the DFS.
				SG.new_search();
				SG.visit(source, { source, 0 });
				dfs = { source, 0 };
				continue;
			}

			if (dfs.i >= SG[dfs.v].size()) {
				if (dfs.v == source) {
					break; // The target is no longer reachable.
				}

				SG.backtrack_dfs(dfs);
				dfs.i++;
				continue;
			}

			vertex w = SG[dfs.v][dfs.i];
			if (SG.visited(w)) {
				dfs.i++;
			}
			else {
				SG.visit(w, dfs);
				dfs = { w, 0 };
			}
		}

		return flow;
	}

}



