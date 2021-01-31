#include "pch.h"

#include "graphgen.h"
#include "random.h"
#include "util.h"

#include <numeric>

namespace graph {
	adjacency_list split_graph(const adjacency_list& G) {
		size_t n = G.size();
		adjacency_list SG(2 * n);
		for (size_t i = 0; i < n; i++) {
			SG[2 * i].push_back(2 * i + 1);
			for (size_t t : G[i])
				SG[2 * i + 1].push_back(2 * t);
		}
		return SG;
	}

	adjacency_list random_forest_graph_with_planted_cut(size_t L, size_t S, size_t R, size_t k) {
		auto& [G, L_set, S_set, R_set] = random_forest_graph_with_planted_cut_and_sets(L, S, R, k);
		return G;
	}

	LSR_and_sets random_forest_graph_with_planted_cut_and_sets(size_t L, size_t S, size_t R, size_t k) {
		size_t N1 = L;
		size_t N2 = L + S;
		size_t N3 = L + S + R;
		std::vector<vertex> all_vertices(N3);
		std::iota(all_vertices.begin(), all_vertices.end(), 0);
		util::random_shuffle(all_vertices);

		struct edge { vertex x, y; };
		std::vector<edge> candidate_edges;
		// L <-> L, S
		for (size_t i = 0; i < N1; i++) {
			vertex x = all_vertices[i];
			for (vertex j = i + 1; j < N2; j++) {
				vertex y = all_vertices[j];
				candidate_edges.push_back({ x, y });
			}
		}
		// S, R <-> S, R
		for (size_t i = N1; i < N3; i++) {
			vertex x = all_vertices[i];
			for (vertex j = i + 1; j < N3; j++) {
				vertex y = all_vertices[j];
				candidate_edges.push_back({ x, y });
			}
		}
		util::random_shuffle(candidate_edges);

		util::multi_union_find forest_sets(N3, k);
		adjacency_list G(N3);

		for (edge& edge : candidate_edges) {
			size_t x = edge.x;
			size_t y = edge.y;
			// i is the lowest possible such that x and y are not in the same tree in F_i.
			size_t i = forest_sets.lowest_not_same(x, y);
			if (i < k) {
				G[x].push_back(y);
				G[y].push_back(x);
				forest_sets.merge(x, y, i);
			}
		}

		vertex_set L_set, S_set, R_set;
		for (size_t i = 0; i < N1; i++)
			L_set.insert(all_vertices[i]);
		for (size_t i = N1; i < N2; i++)
			S_set.insert(all_vertices[i]);
		for (size_t i = N2; i < N3; i++)
			R_set.insert(all_vertices[i]);
		return { G, L_set, S_set, R_set };
	}

	// This might be a problematic family of graphs...
	adjacency_list pathological_forest_graph_with_planted_cut(size_t L, size_t S, size_t R, size_t k) {
		size_t N1 = L;
		size_t N2 = L + S;
		size_t N3 = L + S + R;
		std::vector<vertex> all_vertices(N3);
		std::iota(all_vertices.begin(), all_vertices.end(), 0);
		util::random_shuffle(all_vertices);
		/*
		 * Vertex partition: L, S and R.
		 * L: all_vertices[0  <= i < N1]
		 * S: all_vertices[N1 <= i < N2]
		 * R: all_vertices[N2 <= i < N3]
		 * Vertices in S are fully connected (degree n-1).
		 * There are k-|S| vertices in L and R that are adjacent to every other vertex in their part.
		 * L': all_vertices[N1 - (k-|S|) <= i < N1        ]
		 * R': all_vertices[N2           <= i < N2 + k-|S|]
		 */

		adjacency_list G(N3);
		for (size_t i = 0; i < N3; i++) {
			vertex x = all_vertices[i];

			bool adjacent_to_L = N1 - (k - S) <= i && i < N2;
			bool adjacent_to_R = N1 <= i && i < N2 + (k - S);
			bool in_L = i < N1;
			bool in_R = N2 <= i;

			size_t j_min = adjacent_to_L
					? 0
					: (in_L ? N1 - (k - S) : N1);
			size_t j_max = adjacent_to_R
					? N3
					: (in_R ? N2 + (k - S) : N2);

			for (size_t j = j_min; j < j_max; j++) {
				if (i == j) continue;
				vertex y = all_vertices[j];
				G[x].push_back(y);
			}
		}

		return G;
	}


	adjacency_list random_forest_graph_with_planted_cut_v2(size_t L, size_t S, size_t R, size_t k) {
		if (L + S > k) {
			throw 0; // This generator is not applicable if |L| + |S| is larger than k.
		}

		size_t N1 = R;
		size_t N2 = R + S;
		size_t N3 = R + S + L;
		std::vector<vertex> all_vertices(N3);
		std::iota(all_vertices.begin(), all_vertices.end(), 0);
		util::random_shuffle(all_vertices);

		std::vector<std::pair<vertex, vertex>> edges;
		// All vertices in L are adjacent to all vertices in L and S.
		for (size_t i_x = N3-1; i_x >= N2; i_x--) { // all x in L ...
			vertex x = all_vertices[i_x];
			for (size_t i_y = N1; i_y < i_x; i_y++) { // ... + all y in L,S are adjacent
				vertex y = all_vertices[i_y];
				edges.push_back({ x, y });
			}
		}

		util::multi_union_find forest_sets(N2, k); // Only S and R go through the forest sets.
		// All vertices in S are in the same forest through L for forests 0...(L-1)
		//for (size_t i_y = N1 + 1; i_y < N2; i_y++) {
		//	for(size_t i = 0; i < L; i++)
		//		forest_sets.merge(N1, i_y, i);
		//}

		std::vector<std::unordered_set<vertex>> pseudo_G(N2); // To check for existing edges.

		while (forest_sets.is_nontrivial()) {
			size_t i_x = util::random_int(0, N2 - 1); // x in (S or R)
			size_t i_y = util::random_int(0, N2 - 1); // y in (S or R)
			if (pseudo_G[i_x].find(i_y) != pseudo_G[i_x].end()) continue; // (x, y) already in G. Retry.

			if (i_x > i_y) std::swap(i_x, i_y);
			vertex x = all_vertices[i_x];
			vertex y = all_vertices[i_y];
			size_t i = forest_sets.lowest_not_same(i_x, i_y);

			if (i < k) {
				edges.push_back({ x, y });
				pseudo_G[i_x].insert(i_y);
				pseudo_G[i_y].insert(i_x);
				forest_sets.merge(i_x, i_y, i);
			}
		}

		util::random_shuffle(edges);
		adjacency_list G(N3);
		for (auto& [x, y] : edges) {
			G[x].push_back(y);
			G[y].push_back(x);
		}

		return G;
	}
}
