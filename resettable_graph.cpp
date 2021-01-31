#include "pch.h"

#include "resettable_graph.h"

namespace graph {
	const adjacency_list& resettable_graph::_data() const {
		return adj;
	}

	resettable_graph::const_adj_list& resettable_graph::operator[](index i) const { 
		return adj[i];
	}
	resettable_graph::const_adj_list_iterator resettable_graph::begin() const {
		return adj.begin();
	}
	resettable_graph::const_adj_list_iterator resettable_graph::end() const {
		return adj.end();
	}
	size_t resettable_graph::size() const {
		return adj.size();
	}

	std::ostream& operator<<(std::ostream& os, const resettable_graph& g) {
		for (vertex s = 0; s < g.adj.size(); ++s) {
			os << s << ":";
			for (vertex t : g.adj[s]) {
				os << " " << t;
			}
			os << std::endl;
		}
		return os;
	}

	/* inlined */
	//void resettable_graph::visit(const vertex &v, const resettable_graph::internal_location& from) {
	//	is_visited[v] = true;
	//	visited_vertices.push_back(v);
	//	internal_parent[v] = from;
	//}
	//void resettable_graph::visit(const vertex& v, const vertex& from, const index& from_i) {
	//	is_visited[v] = true;
	//	visited_vertices.push_back(v);
	//	internal_parent[v] = { from, from_i };
	//}
	//bool resettable_graph::visited(const vertex& v) {
	//	return is_visited[v];
	//}
	//void resettable_graph::backtrack_dfs(internal_location& dfs) {
	//	dfs = internal_parent[dfs.v];
	//}

	/* inlined */
	//// Return the number of marked edges.
	//size_t resettable_graph::mark_edges_degreecounting(const vertex& v, const size_t& max) {
	//	if (!is_counted[v]) {
	//		counted_vertices.push_back(v);
	//		uncounted_edges[v] = adj[v].size();
	//		is_counted[v] = true;
	//	}
	//	size_t x = std::min(uncounted_edges[v], max);
	//	uncounted_edges[v] -= x;
	//	return x;
	//}

	/*
	 * original: v_0 -> v_1 -> v_2 -> ... -> v_L
	 * - swap-and-pop (v_0 -> v_1)
	 * - in-place replace (v_{i} -> v_{i+1}) with (v_{i} -> v_{i-1}) 1 <= i <= L-1
	 * - append (v_{L} -> v_{L-1})
	 * result: v_0 <- v_1 <- v_2 <- ... <- v_L
	 * - save indices so everything can be reversed.
	 */
	void resettable_graph::reverse_source_to_vertex(vertex t) {
		if (internal_parent[t].v == t) {
			return; // Do nothing if t is not visited or the root of the current search.
		}

		size_t path_len = 0;
		{
			vertex v = t;
			while (internal_parent[v].v != v) {
				path_len++;
				v = internal_parent[v].v;
			}
		}

		std::vector<internal_location> reversed_path;
		reversed_path.reserve(path_len + 1);
		reversed_path.push_back({ t, 0 });

		vertex s = internal_parent[t].v;
		adj[t].push_back(s); // add (target -> source)
		while (internal_parent[s].v != s) {
			vertex parent = t;
			t = s;
			s = internal_parent[t].v;
			index i = internal_parent[parent].i;

			adj[t][i] = s; // replace (target -> parent) with (target -> source)

			reversed_path.push_back({ t, i });
		}
		index i = internal_parent[t].i;
		adj[s][i] = adj[s].back(); // swap-and-pop (source -> target)
		adj[s].pop_back();
		reversed_path.push_back({ s, i });
		reversal_history.push_back(reversed_path);
	}

	void resettable_graph::new_search() {
		for (vertex v : visited_vertices) {
			is_visited[v] = false;
			// Parents do not need to be reset. Accessing parents of unvisited vertices is undefined behavior.
		}
		visited_vertices.clear();
	}
	void resettable_graph::reset_changes() {
		const bool debug = false;

		while (!reversal_history.empty()) {
			const std::vector<internal_location>& reversed_path = reversal_history.back();
			
			if constexpr (debug) {
				std::cout << "rpath:";
				for (const internal_location& loc : reversed_path)
					std::cout << " " << loc.v;
			}


			if (reversed_path.size() == 1) {
				reversal_history.pop_back();
				continue;
			}

			index i = reversed_path.size() - 1;
			vertex t = reversed_path[reversed_path.size() - 1].v;
			index i_t = reversed_path[reversed_path.size() - 1].i;
			vertex s = reversed_path[reversed_path.size() - 2].v;
			index i_s = reversed_path[reversed_path.size() - 2].i;

			adj[t].push_back(s);					// add (target -> source)
			std::swap(adj[t][i_t], adj[t].back());	// Swap the edge back into its original position.

			for (index i = reversed_path.size() - 2; i > 0; --i) {
				const internal_location& s_loc = reversed_path[i - 1];
				vertex parent = t;
				t = s;
				s = s_loc.v;
				i_t = i_s;
				i_s = s_loc.i;

				assert(adj[t][i_t] == parent);	// (target -> parent) should be the i_t:th edge of source.
				adj[t][i_t] = s;				// Replace (target -> parent) with (target -> source)
			}

			assert(adj[s].back() == t); // (source -> target) should be the last edge of source.
			adj[s].pop_back();			// remove (source -> target)

			reversal_history.pop_back();
		}
		new_search();
		reset_marking();
	}

	void resettable_graph::reset_marking() {
		for (vertex v : counted_vertices) {
			is_counted[v] = false;
		}
		counted_vertices.clear();
	}
};
