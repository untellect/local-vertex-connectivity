#include "pch.h"

#include "localvc.h"
#include "util.h"

namespace vertex_connectivity {
	void unbalancedvc_skip(const adjacency_list& G, resettable_graph& SG, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x_cut, vertex_set& cut, bool& found_cut) { }

	void unbalancedvc_undirected_edgesampling_pickfirst(const adjacency_list& G, resettable_graph& SG, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x_cut, vertex_set& cut, bool& found_cut) {
		if (found_cut) {
			return;
		}

		auto sampler = edge_sampler(G);
		const size_t n = G.size();
		const size_t m = sampler.count_edges();

		for (size_t s = min_vol; s < max_vol; s *= 2) {
			//size_t volume = 2 * s; // We move this into LocalEC instead.
			for (size_t i = 0; i < m; i += s) {
				vertex x = sampler.get_source();
				vertex x_out = 2 * x + 1;

				bool found_new_cut = false;
				localec(SG, x_out, s, k, found_new_cut);
				if (found_new_cut) {
					vertex_set reachable_SG = get_reachable(SG, x_out);
					SG.reset_changes();

					if (reachable_SG.size() >= SG.size() - 2 * k) {
						// Visited (almost) the entire graph.
						// The set R in the separation triple (L,S,R) is empty.
						continue;
					}

					cut = get_unsplit_cut_from_reachable_set(reachable_SG, G[x]); // Using trickery.

					x_cut = x;
					found_cut = true;
					return;
				}
				SG.reset_changes();
			}
		}
	}

	void unbalancedvc_undirected_edgesampling_pickbest(const adjacency_list& G, resettable_graph& SG, size_t k, size_t min_vol, size_t max_vol, localec_fn localec, vertex& x_cut, vertex_set& cut, bool& found_cut) {
		if (found_cut) {
			k = cut.size();
		}

		auto sampler = edge_sampler(G);
		const size_t n = G.size();
		const size_t m = sampler.count_edges();

		for (size_t s = min_vol; s < max_vol; s *= 2) {
			//size_t volume = 2 * s;
			for (size_t i = 0; i < m; i += s) {
				vertex x = sampler.get_source();
				vertex x_out = 2 * x + 1;

				bool found_new_cut = false;
				localec(SG, x_out, s, k, found_new_cut);
				if (found_new_cut) {
					vertex_set reachable_SG = get_reachable(SG, x_out);
					SG.reset_changes();

					if (reachable_SG.size() >= SG.size() - 2 * k) {
						// Visited (almost) the entire graph.
						// The set R in the separation triple (L,S,R) is empty.
						continue;
					}

					cut = get_unsplit_cut_from_reachable_set(reachable_SG, G[x]); // Using trickery.

					x_cut = x;
					found_cut = true;
					k = cut.size();
				}
				SG.reset_changes();
			}
		}
	}
}
