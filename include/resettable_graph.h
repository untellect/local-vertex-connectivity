#pragma once

#include "graph_basics.h"

#include <ostream>

namespace vertex_connectivity {
	class resettable_graph {
	public:
		/* Definitions */
		typedef size_t version;
		typedef size_t index;
		typedef const std::vector<vertex> const_adj_list;
		typedef std::vector<std::vector<vertex>>::const_iterator const_adj_list_iterator;

		struct internal_location {
			vertex v = 0;
			index i = 0;
		}; // Used to refer to adj[v][i] - the i:th vertex adjacent to v.

	private:
	public:
		/* Data structures */
		adjacency_list _adj;

		std::vector<std::vector<internal_location>> _reversal_history; // Contains information on all reversed paths
		std::vector<bool> _is_marked;// Some of its edges are marked.
		std::vector<vertex> _marked_vertices; // Some of theirs edges are marked.
		std::vector<size_t> _unmarked_edges;  // for edge marking degree counting variation

		std::vector<bool> _is_visited;
		std::vector<vertex> _visited_vertices;
		std::vector<internal_location> _parent_location; // parent_location[v] == {w, i} where adj[w][i] = v
		resettable_graph();
	public:
		resettable_graph(const adjacency_list& from);
		static resettable_graph split_graph(const adjacency_list& from);

		//const adjacency_list& get_adj() const;
		//const std::vector<vertex>& get_visited_vertices() const;

		const_adj_list& operator[](index i) const;
		const_adj_list_iterator begin() const;
		const_adj_list_iterator end() const;
		size_t size() const; // number of *vertices*

		friend std::ostream& operator<<(std::ostream& os, const resettable_graph& g);

		// inline void resettable_graph::visit(const vertex& v, const resettable_graph::internal_location& from) {
		// 	is_visited[v] = true;
		// 	visited_vertices.push_back(v);
		// 	internal_parent[v] = from;
		// }
		// inline void resettable_graph::visit(const vertex& v, const vertex& from, const index& from_i) {
		// 	is_visited[v] = true;
		// 	visited_vertices.push_back(v);
		// 	internal_parent[v] = { from, from_i };
		// }
		// inline bool resettable_graph::visited(const vertex& v) {
		// 	return is_visited[v];
		// }
		// inline void resettable_graph::backtrack_dfs(internal_location& dfs) {
		// 	dfs = internal_parent[dfs.v];
		// }

		void visit(const vertex& v); // No parent info.
		void visit(const vertex& v, const internal_location& from);
		// void visit(const vertex& v, const vertex&, const index& from_i);
		bool visited(const vertex& v);
		void backtrack_inplace(internal_location& dfs);

		// inline size_t resettable_graph::mark_edges_degreecounting(const vertex& v, const size_t& max) {
		// 	if (!is_counted[v]) {
		// 		counted_vertices.push_back(v);
		// 		uncounted_edges[v] = adj[v].size();
		// 		is_counted[v] = true;
		// 	}
		// 	size_t x = std::min(uncounted_edges[v], max);
		// 	uncounted_edges[v] -= x;
		// 	return x;
		// }
		size_t mark_edges_degreecounting(const vertex& v, const size_t& max);

		void reverse_source_to_vertex(vertex t);

		void new_search();
		void reset_changes();
		void reset_marking(); // Included in reset_changes
	};
}
