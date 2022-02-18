#include "graphgen.h"
#include "random.h"
#include "util.h"

#include <numeric>

#include <iostream>

namespace vertex_connectivity {
	struct plantedcut_gen_res {
		adjacency_list G;
		vertex_vec L;
		vertex_vec S;
		vertex_vec R;
	};
	plantedcut_gen_res random_forest_graph_with_planted_cut_v1(size_t L, size_t S, size_t R, size_t k) {
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

		return {G,
			{all_vertices.begin(), all_vertices.begin() + N1},
			{all_vertices.begin() + N1, all_vertices.begin() + N2},
			{all_vertices.begin() + N2, all_vertices.end()}
		};
	}

	plantedcut_gen_res random_forest_graph_with_planted_cut_v2(size_t L, size_t S, size_t R, size_t k) {
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

		return {G,
			{all_vertices.begin() + N2, all_vertices.end()},
			{all_vertices.begin() + N1, all_vertices.begin() + N2},
			{all_vertices.begin(), all_vertices.begin() + N1}
		};
	}

	plantedcut_gen_res random_forest_graph_with_planted_cut_v3(size_t L, size_t S, size_t R, size_t k) {
		throw "Not yet implemented.";

		std::vector<size_t> N = {0, L, L+S, L+S+R};
		std::vector<std::pair<vertex, vertex>> edges;
		util::multi_union_find forest_sets(N.back(), k);

		// 0: (L + S)
		// 1: (S + R)
		for(size_t i = 0; i < 2; i++) {
			// TODO handle case where the part is entirely full.
			// TODO handle case where the part is almost full.
			// TODO handle case where the part is almost empty.

			// TODO find a stopping condition that ignores irrelevant vertices.
		}

		std::vector<vertex> all_vertices(N.back());
		std::iota(all_vertices.begin(), all_vertices.end(), 0);
		util::random_shuffle(all_vertices);

		util::random_shuffle(edges);
		adjacency_list G(N.back());
		for (auto& [x, y] : edges) {
			G[all_vertices[x]].push_back(all_vertices[y]);
			G[all_vertices[y]].push_back(all_vertices[x]);
		}

		return {G,
			{all_vertices.begin() + N[0], all_vertices.begin() + N[1]},
			{all_vertices.begin() + N[1], all_vertices.begin() + N[2]},
			{all_vertices.begin() + N[2], all_vertices.end()},
		};
	}

	adjacency_list random_forest_graph_with_planted_cut(size_t L, size_t S, size_t R, size_t k) {
		plantedcut_gen_res res;

		if (L + S > k) {
			// Unable to use v2.
			res = random_forest_graph_with_planted_cut_v1(L, S, R, k);
		}
		else if (S + R < 2 * k) {
			// v1 is probably better suited for these too.
			res = random_forest_graph_with_planted_cut_v1(L, S, R, k);
		}
		else {
			res = random_forest_graph_with_planted_cut_v2(L, S, R, k);
		}

		std::cerr << "L:";
		for(vertex v : res.L)
			std::cerr << " " << v;
		std::cerr << std::endl;
		std::cerr << "S:";
		for(vertex v : res.S)
			std::cerr << " " << v;
		std::cerr << std::endl;
		std::cerr << "R:";
		for(vertex v : res.R)
			std::cerr << " " << v;
		std::cerr << std::endl;

		return res.G;
	}

	struct directed_plantedcut_gen_res {
		adjacency_list G;
		vertex_vec L;
		vertex_vec S_out;
		vertex_vec R;
		vertex_vec S_in;
	};
	directed_plantedcut_gen_res directed_cycles_with_planted_cut_v1(size_t L_size, size_t S_in_size, size_t S_out_size, size_t R_size, size_t k, size_t d_avg) {
		std::vector<size_t> N = {
			0,
			L_size,
			L_size + S_out_size,
			L_size + S_out_size + R_size,
			L_size + S_out_size + R_size + S_in_size
		};
		size_t n = L_size + S_out_size + R_size + S_in_size;

		enum category {
			L = 1,
			S_out = 2,
			R = 4,
			S_in = 8
		};
		std::vector<category> category_of(n);
		for(size_t i = N[0]; i < N[1]; i++)
			category_of[i] = L;
		for(size_t i = N[1]; i < N[2]; i++)
			category_of[i] = S_out;
		for(size_t i = N[2]; i < N[3]; i++)
			category_of[i] = R;
		for(size_t i = N[3]; i < N[4]; i++)
			category_of[i] = S_in;

		std::vector<std::unordered_set<vertex>> pseudo_G(n);
		size_t m = 0;

		// Cycles to guarantee connectivity.
		for(size_t i = 0; i < n; i++) {
			for(size_t dj = 1; dj < k + 1; dj++) {
				size_t j = (i + dj) % n;
				if(category_of[i] == L && (category_of[j] & (R | S_in))) {
					j = (j + R_size + S_in_size) % n; // Skip R and S_in
					if(i == j) break; // Looped around.
				}
				if(category_of[i] == S_out && category_of[j] == L) {
					j = (j + L_size) % n; // Skip L
					if(i == j) break; // Looped around.
				}
				if(category_of[i] == R && (category_of[j] & (L | S_out))) {
					j = (j + L_size + S_out_size) % n; // Skip L and S_out
					if(i == j) break; // Looped around.
				}
				if(category_of[i] == S_in && category_of[j] == R) {
					j = (j + R_size) % n; // Skip R
					if(i == j) break; // Looped around.
				}

				pseudo_G[i].insert(j);
				m++;
			}
		}

		// Additional random edges: Uniform random (i, j)
		if constexpr (true) {
			for(size_t missing_edges = n * d_avg - m; missing_edges; missing_edges--) {
				size_t i, j;
				while(true) {
					i = util::random_int(0, n - 1);
					j = util::random_int(0, n - 2);
					if(j >= i) j++; // This prevents (i != j).
					if(pseudo_G[i].count(j)) continue; // Edge already present.
					if(category_of[i] == L && (category_of[j] & (R | S_in))) continue;
					if(category_of[i] == S_out && category_of[j] == L) continue;
					if(category_of[i] == R && (category_of[j] & (L | S_out))) continue;
					if(category_of[i] == S_in && category_of[j] == R) continue;
					break;
				}
				pseudo_G[i].insert(j);
				m++;
			}
		}
		// Additional random edges: Uniform random i, then j among legal edges
		else if constexpr (true) {
			for(size_t missing_edges = n * d_avg - m; missing_edges; missing_edges--) {
				size_t i = -1, j = -1;
				while(true) {
					i = util::random_int(0, n - 1);
					if(category_of[i] == L)
						j = util::random_int(N[0], N[2] - 2);
					else if(category_of[i] == S_out)
						j = util::random_int(N[1], N[4] - 2);
					else if(category_of[i] == R)
						j = util::random_int(N[2], N[4] - 2);
					else if(category_of[i] == S_in) {
						// sample from N[0] to N[2] and N[3] to N[4], (i == j) is possible in the latter interval.
						j = util::random_int(N[0], n - 2 - R_size);
						if(j >= N[2]) j += R_size;
					}
					if(j >= i) j++; // This prevents (i != j).
					if(pseudo_G[i].count(j)) continue; // Edge already present.
					break;
				}
				pseudo_G[i].insert(j);
				m++;
			}
		}

		// Translate into a random vertex labelling.
		std::vector<vertex> all_vertices(n);
		std::iota(all_vertices.begin(), all_vertices.end(), 0);
		util::random_shuffle(all_vertices);

		directed_plantedcut_gen_res res = {
			adjacency_list(n, vertex_vec()),
			{all_vertices.begin() + N[0], all_vertices.begin() + N[1]},
			{all_vertices.begin() + N[1], all_vertices.begin() + N[2]},
			{all_vertices.begin() + N[2], all_vertices.begin() + N[3]},
			{all_vertices.begin() + N[3], all_vertices.end()},
		};

		for(size_t i = 0; i < n; i++) {
			vertex x = all_vertices[i];
			for(size_t j : pseudo_G[i]) {
				vertex y = all_vertices[j];
				res.G[x].push_back(y);
			}
		}

		for(auto& vec : res.G) {
			util::random_shuffle(vec);
		}

		return res;
	}

	directed_plantedcut_gen_res directed_cycles_with_planted_cut_v2(size_t L_size, size_t S_in_size, size_t S_out_size, size_t k, size_t d_avg) {
		std::vector<size_t> N = {
			0,
			L_size,
			L_size + S_out_size,
			L_size + S_out_size + S_in_size,
		};
		size_t n = L_size + S_out_size + S_in_size;

		enum category {
			L = 0,
			S_out = 1,
			S_in = 2
		};
		std::vector<category> category_of(n);
		for(size_t i = N[0]; i < N[1]; i++)
			category_of[i] = L;
		for(size_t i = N[1]; i < N[2]; i++)
			category_of[i] = S_out;
		for(size_t i = N[2]; i < N[3]; i++)
			category_of[i] = S_in;

		std::vector<std::unordered_set<vertex>> pseudo_G(n);

		// Cycles to guarantee connectivity.
		for(size_t i = 0; i < n; i++) {
			for(size_t dj = 1; dj < k + 1; dj++) {
				size_t j = (i + dj) % n;
				if(category_of[i] == L && (category_of[j] & (S_in))) {
					continue;
				}
				if(category_of[i] == S_out && category_of[j] == L) {
					continue;
				}

				if(i == j) continue;
				pseudo_G[i].insert(j);
			}
		}

		size_t deficiency_L = 0;
		if(S_in_size < k) {
			size_t deficiency_out = k - S_in_size;
			deficiency_L = deficiency_out;

			for(size_t d = deficiency_out; d > 0; d--) {
				size_t i = N[2] - 1 - deficiency_out + d;
				if(i > N[3] || category_of[i] != S_out) continue;
				for(size_t j = N[1]; j <= N[1] + d - 1; j++) {
					if(category_of[j] != S_out) continue;
					if(i == j) continue;
					pseudo_G[i].insert(j);
				}
			}
		}
		if(S_out_size < k) {
			size_t deficiency_in = k - S_out_size;
			if(deficiency_in > deficiency_L)
				deficiency_L = deficiency_in;

			for(size_t d = deficiency_in; d > 0; d--) {
				size_t i = N[3] - 1 - deficiency_in + d;
				if(i > N[3] || category_of[i] != S_in) continue;
				for(size_t j = N[2]; j <= N[2] + d - 1; j++) {
					if(category_of[j] != S_in) continue;
					if(i == j) continue;
					pseudo_G[i].insert(j);
				}
			}
		}
		if(deficiency_L > 0) {
			for(size_t d = deficiency_L; d > 0; d--) {
				size_t i = N[1] - 1 - deficiency_L + d;
				if(i > N[3] || category_of[i] != L) continue;
				for(size_t j = N[0]; j <= N[0] + d - 1; j++) {
					if(category_of[j] != L) continue;
					if(i == j) continue;
					pseudo_G[i].insert(j);
				}
			}
		}

		size_t m = 0;
		for(auto& set : pseudo_G) {
			m += set.size();
		}
		size_t missing_edges = n * d_avg > m ? n * d_avg - m : 0;

		// Additional random edges: Uniform random (i, j)
		if constexpr (true) {
			for(; missing_edges; missing_edges--) {
				size_t i, j;
				while(true) {
					i = util::random_int(0, n - 1);
					j = util::random_int(0, n - 2);
					if(j >= i) j++; // This prevents (i != j).
					if(pseudo_G[i].count(j)) continue; // Edge already present.
					if(category_of[i] == L && (category_of[j] == S_in)) continue;
					if(category_of[i] == S_out && category_of[j] == L) continue;
					break;
				}
				pseudo_G[i].insert(j);
				m++;
			}
		}

		// Translate into a random vertex labelling.
		std::vector<vertex> all_vertices(n);
		std::iota(all_vertices.begin(), all_vertices.end(), 0);
		util::random_shuffle(all_vertices);

		directed_plantedcut_gen_res res = {
			adjacency_list(n, vertex_vec()),
			{all_vertices.begin() + N[0], all_vertices.begin() + N[1]},
			{all_vertices.begin() + N[1], all_vertices.begin() + N[2]},
			{all_vertices.begin() + N[2], all_vertices.begin() + N[3]},
			{all_vertices.begin() + N[3], all_vertices.end()},
		};

		for(size_t i = 0; i < n; i++) {
			vertex x = all_vertices[i];
			for(size_t j : pseudo_G[i]) {
				vertex y = all_vertices[j];
				res.G[x].push_back(y);
			}
		}

		for(auto& vec : res.G) {
			util::random_shuffle(vec);
		}

		return res;
	}

	adjacency_list directed_cycles_with_planted_cut(size_t L, size_t S_in, size_t S_out, size_t R, size_t k, size_t d_avg) {
		// Set R=0 to use the good version.
		directed_plantedcut_gen_res res = (R > 0) ? directed_cycles_with_planted_cut_v1(L, S_in, S_out, R, k, d_avg) : directed_cycles_with_planted_cut_v2(L, S_in, S_out, k, d_avg);

		std::cerr << "L:";
		for(vertex v : res.L)
			std::cerr << " " << v;
		std::cerr << std::endl;
		std::cerr << "S_in:";
		for(vertex v : res.S_in)
			std::cerr << " " << v;
		std::cerr << std::endl;
		std::cerr << "S_out:";
		for(vertex v : res.S_out)
			std::cerr << " " << v;
		std::cerr << std::endl;
		if(R) {
			std::cerr << "R:";
			for(vertex v : res.R)
				std::cerr << " " << v;
			std::cerr << std::endl;
		}

		return res.G;
	}
}