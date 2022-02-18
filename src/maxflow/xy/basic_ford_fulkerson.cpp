#include "vertex_connectivity.h"

#include <iostream>

namespace vertex_connectivity {
	size_t maxflow(resettable_graph& SG, vertex x, vertex y, size_t k) {
		size_t flow = 0;

		SG.new_search();
		SG.visit(x, { x, 0 });
		resettable_graph::internal_location dfs = { x, 0 }; // dfs "current" pointer

		if (k == 0) return 0; // Trivial case.

		while (true) {
			if (dfs.v == y) {
				flow++;
				SG.reverse_source_to_vertex(dfs.v);
				if (flow >= k) return flow; // At most k iterations.

				// Reset the DFS.
				SG.new_search();
				SG.visit(x, { x, 0 });
				dfs = { x, 0 };
				continue;
			}

			if (dfs.i >= SG[dfs.v].size()) {
				if (dfs.v == x) {
					break; // The target is no longer reachable.
				}

				SG.backtrack_inplace(dfs);
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
