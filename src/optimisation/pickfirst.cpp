#include "vertex_connectivity.h"

namespace vertex_connectivity {
	vertex_set vc_optimisation(const adjacency_list& G) {
		bool found_cut = false;
		vertex_set cut;
		for (size_t k = 1; !found_cut; k *= 2) {
			vc_decision(G, k, cut, found_cut);
		}

		return cut;
	}
}

