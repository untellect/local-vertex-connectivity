#include "vertex_connectivity.h"
#include "resettable_graph.h"
#include "util.h"

#include <cmath>

namespace vertex_connectivity {
	void vc_decision(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut) {
		resettable_graph SG = resettable_graph::split_graph(G);
		adjacency_list RG = reverse_graph(G);
		resettable_graph SRG = resettable_graph::split_graph(RG);

		size_t m = 0;
		size_t dmin = SIZE_MAX;
		for (vertex v = 0; v < G.size(); v++) {
			size_t d = G[v].size();
			m += d;
			if (d < dmin) {
				dmin = d;
				if(dmin < k) {
					cut.clear();
					for (vertex u : G[v])
						cut.insert(u);
					found_cut = true;
					k = cut.size();
				}
			}
			d = RG[v].size();
			if (d < dmin) {
				dmin = d;
				if(dmin < k) {
					cut.clear();
					for (vertex u : RG[v])
						cut.insert(u);
					found_cut = true;
					k = cut.size();
				}
			}
		}

		if (m == 0) {
			cut.clear();
			found_cut = true;
			return;
		}

		size_t balanced_repetitions = 3 * k; // m/a = m3k/m = 3k, multiplied by two because of unbalancedvc rounding but divivded by two because we sample two points every time.
		size_t a = m / balanced_repetitions;

		edge_sampler sampler_G(G);
		edge_sampler sampler_RG(RG);

		double boost_factor = 1.0;
		balanced_repetitions = std::ceil(balanced_repetitions * boost_factor);
		balancedvc(G, SG, sampler_G, k, balanced_repetitions, cut, found_cut);
		balancedvc(G, SG, sampler_RG, k, balanced_repetitions, cut, found_cut);
		unbalancedvc(G, SG, sampler_G, k, dmin, a, 1.0 * boost_factor, cut, found_cut);
		unbalancedvc(RG, SRG, sampler_RG, k, dmin, a, 1.0 * boost_factor, cut, found_cut);
	}
}