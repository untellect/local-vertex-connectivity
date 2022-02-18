#include "hrg.h"
#include "random.h"

namespace vertex_connectivity {
	// Warning: This version misbehaves when k is large (near or even above n) because vertices are sampled with repetition.
	void vc_decision(const adjacency_list& G, size_t k, vertex_set& cut, bool& found_cut) {
		const size_t n = G.size();

		double acceptable_err_rate = 0.05;
		double iterations = k < n ? ceil(log(1/acceptable_err_rate) / log((double)n / k)) : k;

		for (size_t i = 0; i < iterations; i++) {
			vertex_set iter_cut;
			bool iter_found = false;
			vertex source = util::random_int(0, n - 1);

			hrg::min_vc_split(G, source, k, iter_cut, iter_found);

			if (iter_found && iter_cut.size() < k) {
				found_cut = true;
				swap(cut, iter_cut);
				k = cut.size();
			}
		}
	}
}
