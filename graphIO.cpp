#include "pch.h"

#include "graphIO.h"

namespace graph {
	/*
	 * Assumes each edge is has only one row "a b" in the file where a > b.
	 * Output is birectional (both directions included)
	 */
	adjacency_list read_adjlist_from_edgelist_undirected(std::ifstream& in, size_t skip_lines) {
		for (size_t i = 0; i < skip_lines; i++)
			in.ignore(10000, '\n');

		size_t n = 0;
		vertex s, t;
		adjacency_list G;
		while (in >> s >> t) {
			if (s >= n) {
				n = s + 1;
				G.resize(n);
			}
			if (t >= n) {
				n = t + 1;
				G.resize(n);
			}

			G[s].push_back(t);
			G[t].push_back(s);
		}

		return G;
	}
	void write_adjlist_to_edgelist_undirected(std::ofstream& out, const adjacency_list& G) {
		size_t n = G.size();

		for (size_t s = 0; s < n; s++) {
			for (size_t t : G[s]) {
				if (s <= t) continue; // Only save edges where s > t.
				out << s << " " << t << std::endl;
			}
		}
	}

}
