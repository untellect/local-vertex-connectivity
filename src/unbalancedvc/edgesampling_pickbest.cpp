#include "vertex_connectivity.h"
#include "util.h"
#include <cmath>

namespace vertex_connectivity {
	void unbalancedvc(const adjacency_list& G, resettable_graph& SG, edge_sampler& sampler, size_t k, size_t min_vol, size_t max_vol, double sample_size_multiplier, vertex_set& cut, bool& found_cut) {
		if (found_cut) {
			k = cut.size();
		}

		const size_t m = sampler.count_edges();

		for (size_t s = min_vol; s < max_vol; s *= 2) {
			size_t sample_size = std::ceil(sample_size_multiplier * m / s);
			for (size_t i = 0; i < sample_size; i++) {
				vertex x = sampler.get_source();
				vertex x_out = 2 * x + 1;
				size_t volume = s;

				bool found_new_cut = false;
				localec(SG, x_out, volume, k, found_new_cut);
				if (found_new_cut) {
					vertex_set reachable_SG = get_reachable(SG, x_out);
					SG.reset_changes();

					if (reachable_SG.size() >= SG.size() - 2 * k) {
						// Visited (almost) the entire graph.
						// The set R in the separation triple (L,S,R) might be empty.
						continue;
					}

					cut = get_unsplit_cut_from_reachable_set(reachable_SG, G[x]); // Using trickery.

					found_cut = true;
					k = cut.size();
				}
				SG.reset_changes();
			}
		}
	}
}

