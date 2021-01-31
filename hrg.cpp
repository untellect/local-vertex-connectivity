#include "pch.h"

#include "hrg.h"
#include "graphgen.h"
#include "random.h"

namespace vertex_connectivity {
	hrg::hrg(const adjacency_list& G, size_t source) {
		n = G.size();
		SG = split_graph(G);
		current_edge_i = std::vector<size_t>(2 * n, 0);

		d = std::vector<size_t>(2 * n);
		W_by_distance = { std::vector<size_t>(), std::vector<size_t>() };
		wbd_i = std::vector<size_t>(2 * n);

		excess = std::vector<size_t>(2 * n);
		best_W = { 2 * ((source + 1) % n) };
		in_W = std::vector<bool>(2 * n, true);
		in_W[2 * source] = false; // Part of the implicitly contracted graph.

		// These vertices are also part of the implicitly contracted graph.
		for (size_t v : SG[2 * source + 1]) {
			// Do not add to W.
			in_W[v] = false;
			// Add to S.
			D[0].push_back(v);
			// Saturate each edge from v.
			for (size_t v2 : SG[v]) {
				SG[v2].push_back(v);
				excess[v2] += 1;
				if (in_W[v2] && excess[v2] == 1) // v2 becomes overflowing
					W_and_overflowing.push_back(v2);
			}
			SG[v].clear();
			excess[v] = 0; // This should *more* than empty the excess at v.
		}

		D[0].push_back(2 * source);
		d_min_D[0] = d_min_D_and_T[0] = 0;
		d_max_D[0] = 1;

		target = 2 * source + 1; // outvertex of s, will be finalised in the first call to new_sink

		for (size_t v = 0; v < 2 * n; v++) {
			if (!in_W[v]) continue; // Skip vertices in the implicitly contracted source vertex (that we add to S in init) except t.

			d[v] = v % 2;
			wbd_i[v] = W_by_distance[d[v]].size();
			W_by_distance[d[v]].push_back(v);
		}
	}

	void hrg::new_sink() {
		auto& wbd = W_by_distance[d[target] - d_min_W];
		// Remove t from W_by_distance using the swap-pop idiom.
		wbd_i[wbd.back()] = wbd_i[target];
		wbd[wbd_i[target]] = wbd.back();
		wbd.pop_back();
		if (wbd.empty()) {
			if (d[target] == d_min_W) {
				W_by_distance.pop_front();
				d_min_W++;
			}
			else if (d[target] == d_min_W + W_by_distance.size() - 1) {
				W_by_distance.pop_back();
			}
		}
		in_W[target] = false;

		// Add t to S.
		D[0].push_back(target);
		if (d[target] < d_min_D[0])
			d_min_D[0] = d[target];
		if (d[target] < d_min_D_and_T[0] && in_T(target))
			d_min_D_and_T[0] = d[target];
		if (d[target] > d_max_D[0])
			d_max_D[0] = d[target];
		// Saturate each edge from t.
		for (size_t v : SG[target]) {
			SG[v].push_back(target);
			excess[v] += 1;
			if (in_W[v] && excess[v] == 1) // v becomes overflowing
				W_and_overflowing.push_back(v);
		}
		SG[target].clear();
		excess[target] = 0; // We are pushing more than the current excess out of the target anyway.

		// Merge D[gamma] if W does not contain a vertex in T.
		if (d_min_W % 2 == 1 && W_by_distance.size() == 1) { // W only contains vertices of a single (odd) distance. (using d-consecutiveness and parity properties)
			// Lazy join without moving vertices from W to D[gamma]
			d_min_W = d_min_D_and_T[gamma] + 1; // Every vertex is in the right W_by_distance partition.
			for (size_t v : W_by_distance[0]) {
				d[v] = d_min_W; // Every vertex has the right recorded distance.
				current_edge_i[v] = 0; // current edge is reset by join
			}

			// Make sure W_by_distance is large enough to merge D[gamma] into W.
			while (d_min_D[gamma] < d_min_W) {
				d_min_W -= 1;
				W_by_distance.push_front({});
			}

			while (d_max_D[gamma] >= d_min_W + W_by_distance.size()) {
				W_by_distance.push_back({});
			}

			// Merge D[gamma] into W.
			for (size_t v : D[gamma]) {
				in_W[v] = true;
				auto& wbd = W_by_distance[d[v] - d_min_W];
				wbd_i[v] = wbd.size();
				wbd.push_back(v);
				if (excess[v] > 0)
					W_and_overflowing.push_back(v);
				current_edge_i[v] = 0;
			}
			// Remove D[gamma]
			D.pop_back();
			d_min_D.pop_back();
			d_max_D.pop_back();
			d_min_D_and_T.pop_back();
			gamma -= 1;
		}

		// Make sure d_min_W is even (the vertices with minimum distance in W are in T)
		if (d_min_W % 2 == 1) {
			// Ensure W_by_distance is the right size
			if (W_by_distance.size() == 2)
				W_by_distance.push_back({});
			// Increase the distance for all vertices in W_by_distance[0]
			for (size_t v : W_by_distance[0]) {
				d[v] += 2;
				wbd_i[v] = W_by_distance[2].size();
				W_by_distance[2].push_back(v);
			}
			W_by_distance.pop_front(); d_min_W += 1;
		}

		// Choose t as a vertex of W with d[t] = d_min_W
		// Should this be a random choice?
		target = W_by_distance[0].front();
	}

	// Push from v along the current edge of v.
	void hrg::push_from(size_t v) {
		const size_t w = SG[v][current_edge_i[v]];
		// Remove edge vw
		SG[v][current_edge_i[v]] = SG[v].back();
		SG[v].pop_back();
		// Add edge wv
		SG[w].push_back(v);
		// Increase excess of w and add it to the queue if it *became* overflowing.
		excess[w] += 1;
		if (excess[w] == 1)
			W_and_overflowing.push_back(w);
		// Reduce excess of v and add it back to the queue if it is still overflowing.
		excess[v] -= 1;
		if (excess[v] > 0)
			W_and_overflowing.push_back(v);
	}

	void hrg::relabel(size_t v) {
		//check("relabel");

		auto& wbd = W_by_distance[d[v] - d_min_W];
		size_t d_max_W = d_min_W + W_by_distance.size() - 1;
		if (wbd.size() == 1) {
			if (d[v] < d_max_W) {
				create_dormant(v);
			}
			else { // d[v] == d_max_W
				create_singleton_dormant_or_join(v);
			}
		}
		else {
			// Scan for an edge vu, u in W.
			size_t d_min_u = SIZE_MAX;
			for (const size_t& u : SG[v]) {
				if (in_W[u] && d_min_u > d[u]) {
					d_min_u = d[u];
				}
			}
			if (d_min_u == SIZE_MAX) { // No residual edge from v to W
				create_singleton_dormant_or_join(v);
			}
			else { // u exists (line 5)
			 // Remove v from W_by_distance, its list is not empty afterwards.
				wbd_i[wbd.back()] = wbd_i[v];
				wbd[wbd_i[v]] = wbd.back();
				wbd.pop_back();
				// Change d[v]
				d[v] = d_min_u + 1;
				// Ensure W_by_distance[d[v] - d_min_W] exists
				while (W_by_distance.size() <= d[v] - d_min_W)
					W_by_distance.push_back({});
				// Add v to W_by_distance
				wbd_i[v] = W_by_distance[d[v] - d_min_W].size();
				W_by_distance[d[v] - d_min_W].push_back(v);
			}
		}
	}

	/* Create dormant set with all u such that d[u] >= d[v] */
	void hrg::create_dormant(size_t v) {
		D.push_back({});
		d_max_D.push_back({ d_min_W + W_by_distance.size() - 1 });
		d_min_D.push_back({ d[v] });
		d_min_D_and_T.push_back({ in_T(d[v]) ? d[v] : d[v] + 1 });
		gamma += 1;
		while (in_W[v]) {
			// Add all vertices u such that d[u] = d_max(W) to D[gamma]
			D[gamma].insert(D[gamma].end(), W_by_distance.back().begin(), W_by_distance.back().end());
			// Remove all such vertices from W
			for (size_t u : W_by_distance.back())
				in_W[u] = false;
			W_by_distance.pop_back();
		}
	}

	/* Create dormant set {v} if v is in T, otherwise join it. */
	/* Relabel line 4 */
	void hrg::create_singleton_dormant_or_join(size_t v) {
		// Remove v from W.
		if (W_by_distance[d[v] - d_min_W].size() == 1) {
			// The only vertex at that distance in W (implies d[v] == d_max(W))
			W_by_distance.pop_back();
		}
		else {
			auto& wbd = W_by_distance[d[v] - d_min_W];
			wbd_i[wbd.back()] = wbd_i[v];
			wbd[wbd_i[v]] = wbd.back();
			wbd.pop_back();
		}
		in_W[v] = false;

		if (in_T(v)) {
			// Add a dormant set {v}
			D.push_back({ v });
			d_min_D.push_back({ d[v] });
			d_max_D.push_back({ d[v] });
			d_min_D_and_T.push_back({ d[v] });
			gamma += 1;
		}
		else {
			// Join v to D[gamma]
			D[gamma].push_back(v);
			d[v] = d_min_D_and_T[gamma] + 1;
			if (d[v] > d_max_D[gamma])
				d_max_D[gamma] = d[v];
		}
	}

	void hrg::min_vc_split_v1(const adjacency_list& G, size_t source, size_t k, vertex_set& cut, bool& found_cut) {
		hrg vc(G, source);
		found_cut = false;

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

				const std::vector<size_t>& adj_v = vc.SG[v];
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

			/*
			 * Calculate the indegree of the set W in the *original* (not residual) split graph.
			 * There is no edge vu in the residual split graph that enters W from a dormant set.
			 * If the edge goes opposite to its original direction it was originally an edge into v (in W)
			 * If both endpoints are in W, then the edge is an edge into W.
			 */
			size_t W_indegree = 0;
			std::vector<size_t> W_neighbors;
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

		vertex_set cut_tmp;
		for (size_t v : vc.best_W_neighbors)
			cut_tmp.insert(v / 2);
		
		found_cut = (cut_tmp.size() < k);

		if (found_cut) {
			swap(cut, cut_tmp);
		}
	}

	void hrg::min_vc_split_v2(const adjacency_list& G, size_t source, size_t k, vertex_set& cut, bool& found_cut) {
		hrg vc(G, source);
		found_cut = false;

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

				const std::vector<size_t>& adj_v = vc.SG[v];
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

			if (vc.excess[vc.target] < vc.best_W_indegree) {
				found_cut = true;
				/*
				 * Calculate the indegree of the set W in the *original* (not residual) split graph.
				 * There is no edge vu in the residual split graph that enters W from a dormant set.
				 * If the edge goes opposite to its original direction it was originally an edge into v (in W)
				 * If both endpoints are in W, then the edge is an edge into W.
				 */
				size_t W_indegree = 0;
				std::vector<size_t> W_neighbors;
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
				}
				else std::cout << "!" << std::endl;
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

		vertex_set cut_tmp;
		for (size_t v : vc.best_W_neighbors)
			cut_tmp.insert(v / 2);

		found_cut = (cut_tmp.size() < k);

		if (found_cut) {
			swap(cut, cut_tmp);
		}
	}

	void hrg::min_vc_split_v3(const adjacency_list& G, size_t source, size_t k, vertex_set& cut, bool& found_cut) {
		hrg vc(G, source);
		found_cut = false;

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

				const std::vector<size_t>& adj_v = vc.SG[v];
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
				std::vector<size_t> W_neighbors;
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

		vertex_set cut_tmp;
		for (size_t v : vc.best_W_neighbors)
			cut_tmp.insert(v / 2);

		// In this version, cut_tmp may be empty if no cut of size <k is found.
		if (cut_tmp.size() >= k)
			found_cut = false;

		if (found_cut) {
			swap(cut, cut_tmp);
		}
	}
}
