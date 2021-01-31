#pragma once

#include "graph_basics.h"

namespace graph {
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

		/* Data structures */
	private:
		adjacency_list adj;

		std::vector<std::vector<internal_location>> reversal_history; // Contains information on all reversed paths
	public:
		std::vector<bool> is_counted;         // has some (possibly all) counted edges.
		std::vector<vertex> counted_vertices; // vertices that have some (possibly all) counted edges.
		std::vector<size_t> uncounted_edges;  // for edge marking degree counting variation

		std::vector<bool> is_visited;
		std::vector<vertex> visited_vertices;
		std::vector<internal_location> internal_parent;

		resettable_graph(const adjacency_list& from) {
			vertex s = 0;
			for (const std::vector<vertex>& vec : from) {
				adj.push_back({});
				for (const vertex& t : vec) {
					adj[s].push_back(t);
				}
				internal_parent.push_back({ 0, 0 });
				is_visited.push_back(false);
				is_counted.push_back(false);
				uncounted_edges.push_back(adj[s].size());

				++s;
			}
		}

		const adjacency_list& _data() const;

		const_adj_list& operator[](index i) const;
		const_adj_list_iterator begin() const;
		const_adj_list_iterator end() const;
		size_t size() const; // number of *vertices*

		friend std::ostream& operator<<(std::ostream& os, const resettable_graph& g);

		inline void resettable_graph::visit(const vertex& v, const resettable_graph::internal_location& from) {
			is_visited[v] = true;
			visited_vertices.push_back(v);
			internal_parent[v] = from;
		}
		inline void resettable_graph::visit(const vertex& v, const vertex& from, const index& from_i) {
			is_visited[v] = true;
			visited_vertices.push_back(v);
			internal_parent[v] = { from, from_i };
		}
		inline bool resettable_graph::visited(const vertex& v) {
			return is_visited[v];
		}
		inline void resettable_graph::backtrack_dfs(internal_location& dfs) {
			dfs = internal_parent[dfs.v];
		}

		//void visit(const vertex& v, const internal_location& from);
		//void visit(const vertex& v, const vertex&, const index& from_i);
		//bool visited(const vertex& v);
		//void backtrack_dfs(internal_location& dfs);

		inline size_t resettable_graph::mark_edges_degreecounting(const vertex& v, const size_t& max) {
			if (!is_counted[v]) {
				counted_vertices.push_back(v);
				uncounted_edges[v] = adj[v].size();
				is_counted[v] = true;
			}
			size_t x = std::min(uncounted_edges[v], max);
			uncounted_edges[v] -= x;
			return x;
		}

		void reverse_source_to_vertex(vertex t);

		void new_search();
		void reset_changes();
		void reset_marking(); // Included in reset_changes
	};
}
