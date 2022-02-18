#include "vertex_connectivity.h"
#include "util.h"

namespace vertex_connectivity {
	void balancedvc(const adjacency_list& G, resettable_graph& SG, edge_sampler& sampler, size_t k, size_t repetitions, vertex_set& cut, bool& found_cut) {
		if (found_cut) {
			k = cut.size();
		}

		for (size_t i = 0; i < repetitions; i++) {
			const vertex x = sampler.get_source();
			const vertex x_out = 2 * x + 1;
			const vertex y_in = 2 * sampler.get_source();

			const size_t flow = maxflow(SG, x_out, y_in, k);

			if (flow < k) {
				// Max xy-flow found. Extract cut from residual graph.
				const vertex_set reachable_SG = get_reachable(SG, x_out);
				SG.reset_changes();
				cut = get_unsplit_cut_from_reachable_set(reachable_SG, G[x]); // Using trickery.
				k = cut.size();
				found_cut = true;
			}
			SG.reset_changes();
		}
	}
}
