#include "hrg.h"

#include "resettable_graph.h"
#include "util.h"

#include <iostream>

namespace vertex_connectivity {
	void hrg::min_vc_split(const adjacency_list& G, size_t source, size_t k, vertex_set& cut, bool& found_cut) {
		if(found_cut && k > cut.size())
			k = cut.size();

		hrg vc(G, source);

		if (vc.W_by_distance.front().size() == 0) {
			// No vertices at d=0 -> No invertices in W -> All invertices contracted into s -> s adjacent to all vertices.
			// We should be choosing only vertices where this is not the case. If not possible, then the connectivity is k-1.
			return;
		}

		while (true) { // exit by break
			vc.new_sink();

			while (!vc.W_and_overflowing.empty()) {
				size_t v = vc.W_and_overflowing.front(); vc.W_and_overflowing.pop_front();
				if (v == vc.target) continue; // t may have become overflowing in new_sink before it was chosen as t. Skip it here.
				if (!vc.in_W[v]) {
					// This is possible because we make (possibly overflowing) vertices dormant in bulk.
					continue;
				}
				// Try to find a residual edge vw with w in W and d[v] = d[w] + 1.
				// While not at the end of the list and the current adjacent vertex does not meet the requirements, increment current edge index

				const std::vector<vertex>& adj_v = vc.SG[v];
				size_t& i = vc.current_edge_i[v];
				while (i < adj_v.size() && (vc.d[adj_v[i]] + 1 != vc.d[v] || !vc.in_W[adj_v[i]])) {
					i++;
				}

				// If we found one (current edge is valid), push one unit of flow.
				if (vc.current_edge_i[v] < vc.SG[v].size()) {
					vc.push_from(v);
				}
				// If we did not find one, relabel.
				else {
					vc.current_edge_i[v] = 0;
					vc.relabel(v);
					if (vc.in_W[v])
						vc.W_and_overflowing.push_back(v);
				}
			}

			if (vc.excess[vc.target] < k && vc.excess[vc.target] < vc.best_W_indegree) {
				found_cut = true;
				/*
				 * Calculate the indegree of the set W in the *original* (not residual) split graph.
				 * There is no edge vu in the residual split graph that enters W from a dormant set.
				 * If the edge goes opposite to its original direction it was originally an edge into v (in W)
				 * If both endpoints are in W, then the edge is an edge into W.
				 */
				size_t W_indegree = 0;
				std::vector<vertex> W_neighbors;
				for (auto& wbd : vc.W_by_distance) {
					for (const size_t& v : wbd) {
						for (const size_t& u : vc.SG[v]) {
							if (vc.in_W[u]) // ignore u in W because then vu is an edge W -> W
								continue;
							bool orig_direction =
								(v % 2 == 0 && v + 1 == u) || // v is an invertex and u is its outvertex
								(v % 2 == 1 && v - 1 != u);   // v is an outvertex and v is not its invertex.

							if (orig_direction) // Ignore edges that go in their original direction because they go out from W, not in, in the original graph.
								continue;
							W_neighbors.push_back(u);
							W_indegree++;

							if (W_indegree > vc.best_W_indegree)
								break;
						}
						if (W_indegree > vc.best_W_indegree)
							break;
					}
					if (W_indegree > vc.best_W_indegree)
						break;
				}

				if (W_indegree < vc.best_W_indegree) { // If W is better than best_W
					vc.best_W_indegree = W_indegree;
					vc.best_W_neighbors = W_neighbors;

					// Clear best_W
					vc.best_W.clear();
					// Add the vertices of W to best_W
					for (auto& wbd : vc.W_by_distance) {
						vc.best_W.insert(vc.best_W.end(), wbd.begin(), wbd.end());
					}

					cut.clear();
					for (size_t v : vc.best_W_neighbors)
						cut.insert(v / 2);

					if(W_indegree != cut.size()) {
						std::cout << W_indegree << " != " << cut.size() << std::endl;
					}
				}
			}

			if (vc.gamma > 0) continue; // If there are dormant sets other than S, then one definitely includes a vertex in T.

			bool exists_T_not_in_S = false;
			for (auto& wbd : vc.W_by_distance) {
				for (size_t v : wbd) {
					if (vc.in_T(v) && v != vc.target) {
						exists_T_not_in_S = true;
						break;
					}
				}
				if (exists_T_not_in_S) break;
			}
			if (!exists_T_not_in_S) break; // End the loop if T is a subset of (S and t)
		}

		// vertex_set cut_tmp;
		// for (size_t v : vc.best_W_neighbors)
		// 	cut_tmp.insert(v / 2);

		// // In this version, cut_tmp may be empty if no cut of size <k is found.
		// if (cut_tmp.size() >= k)
		// 	found_cut = false;

		// if (found_cut) {
		// 	swap(cut, cut_tmp);
		// }
	}
}
