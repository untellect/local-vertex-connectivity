#include "vertex_connectivity.h"

namespace vertex_connectivity {
	vertex_set vc_optimisation(const adjacency_list& G) {
		bool found_cut = false;
		vertex_set cut;
		vc_decision(G, G.size(), cut, found_cut);
		return cut;
	}
}

