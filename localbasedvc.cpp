#include "pch.h"

#include "localvc.h"
#include "graphgen.h"

namespace vertex_connectivity {
	void vertex_connectivity_localbased_edgesampling(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut, localec_fn localec, unbalancedvc_fn unbalancedvc, balancedvc_fn balancedvc) {
		adjacency_list SG_adj = split_graph(G);
		resettable_graph SG(SG_adj);

		size_t n = G.size();
		size_t m = 0;
		for (auto& vec : G)
			m += vec.size();
		size_t a = m / (3 * k);
		size_t balanced_repetitions = 3 * k; // m/a = m3k/m = 3k, multiplied by two because of unbalancedvc rounding but divivded by two because we sample two points every time.

		if (m == 0) {
			cut.clear();
			found_cut = true;
			return;
		}

		balancedvc(G, SG, k, balanced_repetitions, cut, found_cut);
		vertex x; // The seed vertex from where the cut is found. Useful if we need to log the balancedness of the cut.
		unbalancedvc(G, SG, k, 1, a, localec, x, cut, found_cut);
	}

	void vertex_connectivity_localbased_edgesampling_skiptrivialvolume(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut, localec_fn localec, unbalancedvc_fn unbalancedvc, balancedvc_fn balancedvc) {
		constexpr bool debug = false;

		adjacency_list SG_adj = split_graph(G);
		resettable_graph SG(SG_adj);
		
		size_t n = G.size();
		size_t m = 0;
		size_t dmin = SIZE_MAX;
		vertex v_dmin = 0;
		for (vertex v = 0; v < G.size(); v++) {
			size_t d = G[v].size();
			m += d;
			if (d < dmin) {
				dmin = d;
				v_dmin = v;
			}
		}

		if (m == 0) {
			cut.clear();
			found_cut = true;
			return;
		}

		if (dmin < k) {
			cut.clear();
			for (vertex v : G[v_dmin])
				cut.insert(v);
			found_cut = true;
			if constexpr (debug) {
				std::cout << "Found trivial cut: " << cut.size() << "/" << k << std::endl;
			}
			k = cut.size();
			// Do not return here. If we are supposed to use this cut, then balancedvc and unbalancedvc will quit early because found_cut is set.
		}

		size_t a = m / (3 * k);
		size_t balanced_repetitions = 3 * k; // m/a = m3k/m = 3k, multiplied by two because of unbalancedvc rounding but divivded by two because we sample two points every time.

		size_t cutsize = cut.size();
		balancedvc(G, SG, k, balanced_repetitions, cut, found_cut);
		if constexpr (debug) {
			if (cut.size() != cutsize)
				std::cout << "Found cut in balancedvc: " << cut.size() << "/" << k << std::endl;
		}

		cutsize = cut.size();
		vertex x; // The seed vertex from where the cut is found. Useful if we need to log the balancedness of the cut.
		unbalancedvc(G, SG, k, dmin, a, localec, x, cut, found_cut);
		if constexpr (debug) {
			if (cut.size() != cutsize)
				std::cout << "Found cut in unbalancedvc: " << cut.size() << "/" << k << std::endl;
		}
	}
}
