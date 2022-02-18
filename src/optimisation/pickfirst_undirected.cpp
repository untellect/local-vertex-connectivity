#include "vertex_connectivity.h"

namespace vertex_connectivity {
	vertex_set vc_optimisation(const adjacency_list& G) {
		sparsification_labelling labelling = sparsification_labelling::nagamochi_ibaraki(G);
		return vc_optimisation(labelling);
	}
	vertex_set vc_optimisation(sparsification_labelling& labelling) {
		bool found_cut = false;
		vertex_set cut;

		for (size_t k = 1; !found_cut; k *= 2) {
			adjacency_list G = labelling.get_sparsified_undirected_graph(k);
			vc_decision(G, k, cut, found_cut);
		}

		return cut;
	}
}

