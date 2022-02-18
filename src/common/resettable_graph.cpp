#include "resettable_graph.h"

namespace vertex_connectivity {
	resettable_graph::resettable_graph() {}
	resettable_graph::resettable_graph(const adjacency_list& from) {
		vertex s = 0;
		for (const std::vector<vertex>& vec : from) {
			_adj.push_back({});
			for (const vertex& t : vec) {
				_adj[s].push_back(t);
			}
			_parent_location.push_back({ 0, 0 });
			_is_visited.push_back(false);
			_is_marked.push_back(false);
			_unmarked_edges.push_back(_adj[s].size());

			++s;
		}
	}
	resettable_graph resettable_graph::split_graph(const adjacency_list& from) {
		resettable_graph SG;
		vertex s = 0;
		for (const std::vector<vertex>& vec : from) {
			// Invertex
			SG._adj.push_back({s+1});
			SG._parent_location.push_back({ 0, 0 });
			SG._is_visited.push_back(false);
			SG._is_marked.push_back(false);
			SG._unmarked_edges.push_back(SG._adj[s].size());
			++s;
			// Outvertex
			SG._adj.push_back({});
			for (const vertex& t : vec) {
				SG._adj.back().push_back(2 * t);
			}
			SG._parent_location.push_back({ 0, 0 });
			SG._is_visited.push_back(false);
			SG._is_marked.push_back(false);
			SG._unmarked_edges.push_back(SG._adj[s].size());
			++s;
		}
		return SG;
	}

	// const adjacency_list& resettable_graph::get_adj() const {
	// 	return _adj;
	// }

	// const std::vector<vertex>& resettable_graph::get_visited_vertices() const {
	// 	return _visited_vertices;
	// }

	resettable_graph::const_adj_list& resettable_graph::operator[](index i) const { 
		return _adj[i];
	}
	resettable_graph::const_adj_list_iterator resettable_graph::begin() const {
		return _adj.begin();
	}
	resettable_graph::const_adj_list_iterator resettable_graph::end() const {
		return _adj.end();
	}
	size_t resettable_graph::size() const {
		return _adj.size();
	}

	std::ostream& operator<<(std::ostream& os, const resettable_graph& g) {
		for (vertex s = 0; s < g._adj.size(); ++s) {
			os << s << ":";
			for (vertex t : g._adj[s]) {
				os << " " << t;
			}
			os << std::endl;
		}
		return os;
	}

	void resettable_graph::visit(const vertex &v, const resettable_graph::internal_location& from) {
		_is_visited[v] = true;
		_visited_vertices.push_back(v);
		_parent_location[v] = from;
	}
	void resettable_graph::visit(const vertex& v) {
		_is_visited[v] = true;
		_visited_vertices.push_back(v);
	}
	bool resettable_graph::visited(const vertex& v) {
		return _is_visited[v];
	}
	void resettable_graph::backtrack_inplace(internal_location& dfs) {
		dfs = _parent_location[dfs.v];
	}

	/* inlined */
	// Return the number of marked edges.
	size_t resettable_graph::mark_edges_degreecounting(const vertex& v, const size_t& max) {
		if (!_is_marked[v]) {
			_marked_vertices.push_back(v);
			_is_marked[v] = true;
		}
		size_t x = std::min(_unmarked_edges[v], max);
		_unmarked_edges[v] -= x;
		return x;
	}

	/*
	 * original: v_0 -> v_1 -> v_2 -> ... -> v_L
	 * - swap-and-pop (v_0 -> v_1)
	 * - in-place replace (v_{i} -> v_{i+1}) with (v_{i} -> v_{i-1}) 1 <= i <= L-1
	 * - append (v_{L} -> v_{L-1})
	 * result: v_0 <- v_1 <- v_2 <- ... <- v_L
	 * - save indices so everything can be reversed.
	 */
	void resettable_graph::reverse_source_to_vertex(vertex t) {
		if (_parent_location[t].v == t) {
			return; // Do nothing if t is not visited or the root of the current search.
		}

		size_t path_len = 0;
		{
			vertex v = t;
			while (_parent_location[v].v != v) {
				path_len++;
				v = _parent_location[v].v;
			}
		}

		std::vector<internal_location> reversed_path;
		reversed_path.reserve(path_len + 1);
		reversed_path.push_back({ t, 0 });

		vertex s = _parent_location[t].v;
		_adj[t].push_back(s); // add (target -> source)
		while (_parent_location[s].v != s) {
			vertex parent = t;
			t = s;
			s = _parent_location[t].v;
			index i = _parent_location[parent].i;

			_adj[t][i] = s; // replace (target -> parent) with (target -> source)

			reversed_path.push_back({ t, i });
		}
		index i = _parent_location[t].i;
		_adj[s][i] = _adj[s].back(); // swap-and-pop (source -> target)
		_adj[s].pop_back();
		reversed_path.push_back({ s, i });
		_reversal_history.push_back(reversed_path);
	}

	void resettable_graph::new_search() {
		for (vertex v : _visited_vertices) {
			_is_visited[v] = false;
			// Parents do not need to be reset. Accessing parents of unvisited vertices is undefined behavior.
		}
		_visited_vertices.clear();
	}
	void resettable_graph::reset_changes() {
		while (!_reversal_history.empty()) {
			const std::vector<internal_location>& reversed_path = _reversal_history.back();
			
			if (reversed_path.size() == 1) {
				_reversal_history.pop_back();
				continue;
			}

			vertex t = reversed_path[reversed_path.size() - 1].v;
			index i_t = reversed_path[reversed_path.size() - 1].i;
			vertex s = reversed_path[reversed_path.size() - 2].v;
			index i_s = reversed_path[reversed_path.size() - 2].i;

			_adj[t].push_back(s);					// add (target -> source)
			std::swap(_adj[t][i_t], _adj[t].back());	// Swap the edge back into its original position.

			for (index i = reversed_path.size() - 2; i > 0; --i) {
				const internal_location& s_loc = reversed_path[i - 1];
				t = s;
				s = s_loc.v;
				i_t = i_s;
				i_s = s_loc.i;
				_adj[t][i_t] = s;				// Replace (target -> parent) with (target -> source)
			}
			_adj[s].pop_back();			// remove (source -> target)

			_reversal_history.pop_back();
		}
		new_search();
		reset_marking();
	}

	void resettable_graph::reset_marking() {
		for (vertex v : _marked_vertices) {
			_unmarked_edges[v] = _adj[v].size();
			_is_marked[v] = false;
		}
		_marked_vertices.clear();
	}
};
