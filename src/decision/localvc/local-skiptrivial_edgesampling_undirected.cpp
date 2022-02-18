#include "vertex_connectivity.h"
#include "resettable_graph.h"

#include <cmath>

namespace vertex_connectivity {
	void vc_decision(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut) {
		resettable_graph SG = resettable_graph::split_graph(G);
		
		size_t dmin = SIZE_MAX;
		vertex v_dmin = 0;
		for (vertex v = 0; v < G.size(); v++) {
			size_t d = G[v].size();
			if (d < dmin) {
				dmin = d;
				v_dmin = v;
			}
			if(dmin < k) {
				cut.clear();
				for (vertex v : G[v_dmin])
					cut.insert(v);
				found_cut = true;
				k = cut.size();
			}
		}

		edge_sampler sampler_G(G);

		const size_t m = sampler_G.count_edges();
		if (m == 0) {
			cut.clear();
			found_cut = true;
			return;
		}

		size_t balanced_repetitions = 3 * k; // m/a = m3k/m = 3k, multiplied by two because of unbalancedvc rounding but divivded by two because we sample two points every time.
		size_t a = m / balanced_repetitions;

		double boost_factor = 1.0;
		balanced_repetitions = std::ceil(balanced_repetitions * boost_factor);
		balancedvc(G, SG, sampler_G, k, balanced_repetitions, cut, found_cut);
		unbalancedvc(G, SG, sampler_G, k, dmin, a, 1.0 * boost_factor, cut, found_cut);
	}
}