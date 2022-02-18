#include "hrg.h"
#include "random.h"
#include "util.h"

#include <iostream>

namespace vertex_connectivity {
	// Warning: This version misbehaves when k is large (near or even above n) because vertices are sampled with repetition.
	void vc_decision(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut) {
		const adjacency_list RG = reverse_graph(G);
		const size_t n = G.size();

		size_t d_min = G.size();
		for(vertex v = 0; v < n; v++){
			if(d_min > G[v].size())
				d_min = G[v].size();
			if(d_min > RG[v].size())
				d_min = RG[v].size();
		}
		if(k > d_min)
			k = d_min + 1;

		double acceptable_err_rate = 0.05;
		double iterations = k < n ? ceil(log(1/acceptable_err_rate) / log((double)n / k)) : k;
		if(iterations > k)
			iterations = k;

		for (size_t i = 0; i < iterations; i++) {
			vertex source = util::random_int(0, n - 1);

			hrg::min_vc_split(G, source, k, cut, found_cut);
			hrg::min_vc_split(RG, source, k, cut, found_cut);
		}
	}
}
