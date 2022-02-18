#include "vertex_connectivity.h"

namespace vertex_connectivity {
	size_t maxflow(resettable_graph& SG, vertex_vec& S, std::vector<bool> in_T) {
		size_t flow = 0;

		for(vertex s : S) {
			flow += maxflow(SG, s, in_T); // Do not reset between these.
		}

		return flow;
	}
}
